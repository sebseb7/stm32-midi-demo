#ifndef MAIN_H_
#define MAIN_H_

#if STM32F == 2
#include "stm32f2xx.h"
#endif
#if STM32F == 4
#include "stm32f4xx.h"
#endif

#define KEY_A (1<<0)
#define KEY_B (1<<1)

#endif
