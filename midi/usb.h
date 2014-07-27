// $Id: usb.h 1715 2013-03-17 11:14:14Z tk $
/*
 * Header file for USB Driver
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _USB_H
#define _USB_H

#include "main.h"

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


// Following settings allow to customize the USB device descriptor
#ifndef USB_VENDOR_ID
#define USB_VENDOR_ID    0x16c0        // sponsored by voti.nl! see http://www.voti.nl/pids
#endif
#ifndef USB_VENDOR_STR
#define USB_VENDOR_STR   "seb" // you will see this in the USB device description
#endif
#ifndef USB_PRODUCT_STR
#define USB_PRODUCT_STR  "midi_ctrl"      // you will see this in the MIDI device list
#endif
#ifndef USB_PRODUCT_ID
#define USB_PRODUCT_ID   0x03e8        // ==1022; 1020-1029 reserved for T.Klose, 1000 - 1009 free for lab use
                                              // note: Vendor ID 1022 is required if the GM5 driver should be used!
#endif
#ifndef USB_VERSION_ID
#define USB_VERSION_ID   0x0100        // v1.00
#endif

#define USB_MIDI_NUM_PORTS 1

// internal defines which are used by MIOS32 USB MIDI/COM (don't touch)
#define USB_EP_NUM   5

// buffer table base address
#define USB_BTABLE_ADDRESS      0x000

// EP0 rx/tx buffer base address
#define USB_ENDP0_RXADDR        0x040
#define USB_ENDP0_TXADDR        0x080

// EP1 Rx/Tx buffer base address for MIDI driver
#define USB_ENDP1_TXADDR        0x0c0
#define USB_ENDP2_RXADDR        0x100

// EP3/4/5 buffer base addresses for COM driver
#define USB_ENDP3_RXADDR        0x140
#define USB_ENDP4_TXADDR        0x180
#define USB_ENDP5_TXADDR        0x1c0



/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 USB_Init(u32 mode);
extern s32 USB_IsInitialized(void);
extern s32 USB_ForceSingleUSB(void);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////

extern void (*pEpInt_IN[7])(void);
extern void (*pEpInt_OUT[7])(void);

#endif /* _USB_H */
