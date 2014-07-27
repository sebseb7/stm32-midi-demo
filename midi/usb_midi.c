// $Id: usb_midi.c 2025 2014-07-06 18:22:18Z tk $
//! \defgroup USB_MIDI
//!
//! USB MIDI layer for MIOS32
//! 
//! Applications shouldn't call these functions directly, instead please use \ref MIDI layer functions
//! 
//! \{
/* ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <usb.h>
#include <usb_midi.h>

#include "libs/irq.h"

#include <usb_core.h>
#include <usbd_req.h>
#include <usb_regs.h>


// imported from usb.c
extern USB_OTG_CORE_HANDLE  USB_OTG_dev;
extern uint32_t USB_rx_buffer[USB_MIDI_DATA_OUT_SIZE/4];
static uint32_t USB_tx_buffer[USB_MIDI_DATA_IN_SIZE/4];


/////////////////////////////////////////////////////////////////////////////
// Local prototypes
/////////////////////////////////////////////////////////////////////////////

static void USB_MIDI_TxBufferHandler(void);
static void USB_MIDI_RxBufferHandler(void);


/////////////////////////////////////////////////////////////////////////////
// Local Variables
/////////////////////////////////////////////////////////////////////////////

// Rx buffer
static u32 rx_buffer[USB_MIDI_RX_BUFFER_SIZE];
static volatile u16 rx_buffer_tail;
static volatile u16 rx_buffer_head;
static volatile u16 rx_buffer_size;
static volatile u8 rx_buffer_new_data;

// Tx buffer
static u32 tx_buffer[USB_MIDI_TX_BUFFER_SIZE];
static volatile u16 tx_buffer_tail;
static volatile u16 tx_buffer_head;
static volatile u16 tx_buffer_size;
static volatile u8 tx_buffer_busy;

// transfer possible?
static u8 transfer_possible = 0;


/////////////////////////////////////////////////////////////////////////////
//! Initializes USB MIDI layer
//! \param[in] mode currently only mode 0 supported
//! \return < 0 if initialisation failed
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_Init(u32 mode)
{
  // currently only mode 0 supported
  if( mode != 0 )
    return -1; // unsupported mode

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
//! This function is called by the USB driver on cable connection/disconnection
//! \param[in] connected status (1 if connected)
//! \return < 0 on errors
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_ChangeConnectionState(u8 connected)
{
  // in all cases: re-initialize USB MIDI driver
  // clear buffer counters and busy/wait signals again (e.g., so that no invalid data will be sent out)
  rx_buffer_tail = rx_buffer_head = rx_buffer_size = 0;
  rx_buffer_new_data = 0; // no data received yet
  tx_buffer_tail = tx_buffer_head = tx_buffer_size = 0;

  if( connected ) {
    transfer_possible = 1;
    tx_buffer_busy = 0; // buffer not busy anymore

  } else {
    // cable disconnected: disable transfers
    transfer_possible = 0;
    tx_buffer_busy = 1; // buffer busy
  }

  return 0; // no error
}

/////////////////////////////////////////////////////////////////////////////
//! This function returns the connection status of the USB MIDI interface
//! \param[in] cable number
//! \return 1: interface available
//! \return 0: interface not available
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_CheckAvailable(u8 cable)
{
  if( cable >= USB_MIDI_NUM_PORTS )
    return 0;

  return transfer_possible ? 1 : 0;
}


/////////////////////////////////////////////////////////////////////////////
//! This function puts a new MIDI package into the Tx buffer
//! \param[in] package MIDI package
//! \return 0: no error
//! \return -1: USB not connected
//! \return -2: buffer is full
//!             caller should retry until buffer is free again
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_PackageSend_NonBlocking(midi_package_t package)
{
  // device available?
  if( !transfer_possible )
    return -1;

  // buffer full?
  if( tx_buffer_size >= (USB_MIDI_TX_BUFFER_SIZE-1) ) {
    // call USB handler, so that we are able to get the buffer free again on next execution
    // (this call simplifies polling loops!)
    USB_MIDI_TxBufferHandler();

    // device still available?
    // (ensures that polling loop terminates if cable has been disconnected)
    if( !transfer_possible )
      return -1;

    // notify that buffer was full (request retry)
    return -2;
  }

  // put package into buffer - this operation should be atomic!
  IRQ_Disable();
  tx_buffer[tx_buffer_head++] = package.ALL;
  if( tx_buffer_head >= USB_MIDI_TX_BUFFER_SIZE )
    tx_buffer_head = 0;
  ++tx_buffer_size;
  IRQ_Enable();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
//! This function puts a new MIDI package into the Tx buffer
//! (blocking function)
//! \param[in] package MIDI package
//! \return 0: no error
//! \return -1: USB not connected
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_PackageSend(midi_package_t package)
{
  static u16 timeout_ctr = 0;
  // this function could hang up if USB is available, but MIDI port won't be
  // serviced by the host (e.g. windows: no program uses the MIDI IN port)
  // Therefore we time out the polling after 10000 tries
  // Once the timeout value is reached, each new MIDI_PackageSend call will
  // try to access the USB port only a single time anymore. Once the try
  // was successfull (MIDI port will be used by host), timeout value is
  // reset again

  s32 error;

  while( (error=USB_MIDI_PackageSend_NonBlocking(package)) == -2 ) {
    if( timeout_ctr >= 10000 )
      break;
    ++timeout_ctr;
  }

  if( error >= 0 ) // no error: reset timeout counter
    timeout_ctr = 0;

  return error;
}


/////////////////////////////////////////////////////////////////////////////
//! This function checks for a new package
//! \param[out] package pointer to MIDI package (received package will be put into the given variable)
//! \return -1 if no package in buffer
//! \return >= 0: number of packages which are still in the buffer
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_PackageReceive(midi_package_t *package)
{
  // package received?
  if( !rx_buffer_size )
    return -1;

  // get package - this operation should be atomic!
  IRQ_Disable();
  package->ALL = rx_buffer[rx_buffer_tail];
  if( ++rx_buffer_tail >= USB_MIDI_RX_BUFFER_SIZE )
    rx_buffer_tail = 0;
  --rx_buffer_size;
  IRQ_Enable();

  return rx_buffer_size;
}



/////////////////////////////////////////////////////////////////////////////
//! This function should be called periodically each mS to handle timeout
//! and expire counters.
//!
//! For USB MIDI it also checks for incoming/outgoing USB packages!
//!
//! Not for use in an application - this function is called from
//! MIDI_Periodic_mS(), which is called by a task in the programming
//! model!
//! 
//! \return < 0 on errors
/////////////////////////////////////////////////////////////////////////////
s32 USB_MIDI_Periodic_mS(void)
{

  USB_MIDI_RxBufferHandler();

  // check for packages which should be transmitted
  USB_MIDI_TxBufferHandler();

  return 0;
}


/////////////////////////////////////////////////////////////////////////////
//! USB Device Mode
//!
//! This handler sends the new packages through the IN pipe if the buffer 
//! is not empty
/////////////////////////////////////////////////////////////////////////////
static void USB_MIDI_TxBufferHandler(void)
{
  // before using the handle: ensure that device (and class) already configured
  if( USB_OTG_dev.dev.class_cb == NULL )
    return;

  // send buffered packages if
  //   - last transfer finished
  //   - new packages are in the buffer
  //   - the device is configured

  // atomic operation to avoid conflict with other interrupts
  IRQ_Disable();

  if( !tx_buffer_busy && tx_buffer_size && transfer_possible ) {
    s16 count = (tx_buffer_size > (USB_MIDI_DATA_IN_SIZE/4)) ? (USB_MIDI_DATA_IN_SIZE/4) : tx_buffer_size;

    // notify that new package is sent
    tx_buffer_busy = 1;

    // send to IN pipe
    tx_buffer_size -= count;

    u32 *buf_addr = (u32 *)USB_tx_buffer;
    int i;
    for(i=0; i<count; ++i) {
      *(buf_addr++) = tx_buffer[tx_buffer_tail];
      if( ++tx_buffer_tail >= USB_MIDI_TX_BUFFER_SIZE )
	tx_buffer_tail = 0;
    }

    DCD_EP_Tx(&USB_OTG_dev, USB_MIDI_DATA_IN_EP, (uint8_t*)&USB_tx_buffer, count*4);
  }

  IRQ_Enable();
}


/////////////////////////////////////////////////////////////////////////////
//! USB Device Mode
//!
//! This handler receives new packages if the Tx buffer is not full
/////////////////////////////////////////////////////////////////////////////
static void USB_MIDI_RxBufferHandler(void)
{
  s16 count;

  // before using the handle: ensure that device (and class) already configured
  if( USB_OTG_dev.dev.class_cb == NULL ) {
    return;
  }

  // atomic operation to avoid conflict with other interrupts
  IRQ_Disable();

  // check if we can receive new data and get packages to be received from OUT pipe
  u32 ep_num = USB_MIDI_DATA_OUT_EP & 0x7f;
  USB_OTG_EP *ep = &USB_OTG_dev.dev.out_ep[ep_num];
  if( rx_buffer_new_data && (count=ep->xfer_count>>2) ) {
    // check if buffer is free
    if( count < (USB_MIDI_RX_BUFFER_SIZE-rx_buffer_size) ) {
      u32 *buf_addr = (u32 *)USB_rx_buffer;

      // copy received packages into receive buffer
      // this operation should be atomic
      do {
	midi_package_t package;
	package.ALL = *buf_addr++;

	//if( MIDI_SendPackageToRxCallback(USB0 + package.cable, package) == 0 ) 
	{
	  rx_buffer[rx_buffer_head] = package.ALL;

	  if( ++rx_buffer_head >= USB_MIDI_RX_BUFFER_SIZE )
	    rx_buffer_head = 0;
	  ++rx_buffer_size;
	}
      } while( --count > 0 );

      // notify, that data has been put into buffer
      rx_buffer_new_data = 0;

      // configuration for next transfer
      DCD_EP_PrepareRx(&USB_OTG_dev,
		       USB_MIDI_DATA_OUT_EP,
		       (uint8_t*)(USB_rx_buffer),
		       USB_MIDI_DATA_OUT_SIZE);
    }
  }

  IRQ_Enable();
}


/////////////////////////////////////////////////////////////////////////////
//! Called by STM32 USB Device driver to check for IN streams
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
//! \note also: bEP, bEPStatus only relevant for LPC17xx port
/////////////////////////////////////////////////////////////////////////////
void USB_MIDI_EP1_IN_Callback(u8 bEP __attribute__((__unused__)), u8 bEPStatus __attribute__((__unused__)))
{
  // package has been sent
  tx_buffer_busy = 0;

  // check for next package
  USB_MIDI_TxBufferHandler();
}

/////////////////////////////////////////////////////////////////////////////
//! Called by STM32 USB Device driver to check for OUT streams
//! \note Applications shouldn't call this function directly, instead please use \ref MIDI layer functions
//! \note also: bEP, bEPStatus only relevant for LPC17xx port
/////////////////////////////////////////////////////////////////////////////
void USB_MIDI_EP2_OUT_Callback(u8 bEP __attribute__((__unused__)), u8 bEPStatus __attribute__((__unused__)))
{
  // put package into buffer
  rx_buffer_new_data = 1;
  USB_MIDI_RxBufferHandler();
}


