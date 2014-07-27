#ifndef _IRQ_H
#define _IRQ_H

#include "main.h"

#define IRQ_USB_PRIORITY	8


void IRQ_Disable(void);
int32_t IRQ_Enable(void);

int32_t IRQ_Install(uint8_t IRQn, uint8_t priority);
void IRQ_DeInstall(uint8_t IRQn);

#endif 
