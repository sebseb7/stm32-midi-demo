// $Id: usb_midi.h 1800 2013-06-02 22:09:03Z tk $
/*
 * Header file for USB MIDI Driver
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _USB_MIDI_H
#define _USB_MIDI_H

#include "midi.h"

/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////

// 1 to stay compatible to USB MIDI spec, 0 as workaround for some windows versions...
#ifndef USB_MIDI_USE_AC_INTERFACE
#define USB_MIDI_USE_AC_INTERFACE 0
#endif

// allowed numbers: 1..8
#ifndef USB_MIDI_NUM_PORTS
#define USB_MIDI_NUM_PORTS 1
#endif

// buffer size (should be at least >= USB_MIDI_DESC_DATA_*_SIZE/4)
#ifndef USB_MIDI_RX_BUFFER_SIZE
#define USB_MIDI_RX_BUFFER_SIZE   64 // packages
#endif

#ifndef USB_MIDI_TX_BUFFER_SIZE
#define USB_MIDI_TX_BUFFER_SIZE   64 // packages
#endif


// size of IN/OUT pipe
#ifndef USB_MIDI_DATA_IN_SIZE
#define USB_MIDI_DATA_IN_SIZE           64
#endif
#ifndef USB_MIDI_DATA_OUT_SIZE
#define USB_MIDI_DATA_OUT_SIZE          64
#endif


// endpoint assignments (don't change!)
#define USB_MIDI_DATA_OUT_EP 0x02
#define USB_MIDI_DATA_IN_EP  0x81


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 USB_MIDI_Init(u32 mode);

extern s32 USB_MIDI_ChangeConnectionState(u8 connected);
extern void USB_MIDI_EP1_IN_Callback(u8 bEP, u8 bEPStatus);
extern void USB_MIDI_EP2_OUT_Callback(u8 bEP, u8 bEPStatus);

extern s32 USB_MIDI_CheckAvailable(u8 cable);

extern s32 USB_MIDI_PackageSend_NonBlocking(midi_package_t package);
extern s32 USB_MIDI_PackageSend(midi_package_t package);
extern s32 USB_MIDI_PackageReceive(midi_package_t *package);

extern s32 USB_MIDI_Periodic_mS(void);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////


#endif /* _USB_MIDI_H */
