#include <libs/delay.h>

#define DELAY_TIMER  TIM1

#define DELAY_TIMER_RCC RCC_APB2Periph_TIM1


void DELAY_Init(void)
{

	// enable timer clock
	if( DELAY_TIMER == TIM1 || DELAY_TIMER == TIM8 )
		RCC_APB2PeriphClockCmd(DELAY_TIMER_RCC, ENABLE);
	else
		RCC_APB1PeriphClockCmd(DELAY_TIMER_RCC, ENABLE);


	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// time base configuration
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 65535; // maximum value
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.SYSCLK_Frequency/1000000)-1; // for 1 uS accuracy
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(DELAY_TIMER, &TIM_TimeBaseStructure);

	// enable counter
	TIM_Cmd(DELAY_TIMER, ENABLE);

}

void DELAY_Wait_uS(uint16_t uS)
{
	uint16_t start = DELAY_TIMER->CNT;

	// note that this even works on 16bit counter wrap-arounds
	while( (uint16_t)(DELAY_TIMER->CNT - start) <= uS );

}


