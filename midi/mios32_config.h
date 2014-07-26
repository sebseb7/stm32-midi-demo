// $Id: mios32_config.h 1973 2014-03-15 17:32:20Z tk $
/*
 * Local MIOS32 configuration file
 *
 * this file allows to disable (or re-configure) default functions of MIOS32
 * available switches are listed in $MIOS32_PATH/modules/mios32/MIOS32_CONFIG.txt
 *
 */

#ifndef _MIOS32_CONFIG_H
#define _MIOS32_CONFIG_H

#define MIOS32_DONT_USE_UART
#define MIOS32_DONT_USE_IIC
#define MIOS32_DONT_USE_SPI
#define MIOS32_DONT_USE_UART_MIDI
#define MIOS32_DONT_USE_IIC_MIDI
#define MIOS32_DONT_USE_SPI_MIDI
#define MIOS32_DONT_USE_USB_HOST

// The boot message which is print during startup and returned on a SysEx query

// how many SPI MIDI ports are available?
// if 0: interface disabled (default)
// other allowed values: 1..8

// enable 4 USB MIDI ports
#define MIOS32_USB_MIDI_NUM_PORTS 1


#endif /* _MIOS32_CONFIG_H */
