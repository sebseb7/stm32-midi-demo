#include "main.h"
#include "libs/math_emb.h"

#include "libs/spi.h"

#include "usb.h"
//#include "midi.h"
#include "libs/delay.h"
#include "usb_midi.h"


/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *  data sheet : http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00277537.pdf
 *
 */

static uint16_t key_state;
static uint16_t key_press;
static uint32_t buttonsInitialized = 0;

void SysTick_Handler(void)
{
	static uint16_t ct0, ct1;
	static uint16_t button_event = 3;
	uint16_t i;

	USB_MIDI_Periodic_mS();
	
	if(buttonsInitialized)
	{
		button_event--;
		if(button_event == 0)
		{
			uint16_t key_curr = ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)<<1)|
								  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6));
	
			i = key_state ^ ~key_curr;
			ct0 = ~( ct0 & i );
			ct1 = ct0 ^ (ct1 & i);
			i &= ct0 & ct1;
			key_state ^= i;
			key_press |= key_state & i;
			button_event=3;
		}
	}
}
uint16_t get_key_press( uint16_t key_mask )
{
	key_mask &= key_press;                          // read key(s)
	key_press ^= key_mask;                          // clear key(s)
	return key_mask;
}

uint16_t get_key_state( uint16_t key_mask )
{
	return key_mask & key_press;
}


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick event each 1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

	DELAY_Init();
	USB_Init(0);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;       
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;       
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
	//buttons
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	buttonsInitialized=1;
	
	int loopcount = 0;
	while(1)
	{
		midi_package_t rpack;

		int recv = USB_MIDI_PackageReceive(&rpack);

		if(recv != -1)
		{
			if(rpack.velocity > 50)
			{
				GPIOB->ODR           |=       1<<13;
			}
			else
			{
				GPIOB->ODR           &=       ~(1<<13);
			}
		}

		loopcount++;
		if((loopcount == 50)||(loopcount == 150))
		{
			if(USB_MIDI_CheckAvailable(0))
			{
				GPIOB->ODR           &=       ~(1<<12);
			}
		}
		if((loopcount == 100)||(loopcount == 200))
		{
			if(USB_MIDI_CheckAvailable(0))
			{
				GPIOB->ODR           |=       1<<12;
			}

			if(loopcount==200)
				loopcount = 0;
  		
		}
		
		if(get_key_press(KEY_A))
		{
		
			midi_package_t package;

			package.type     = CC;
			package.event    = CC;
			package.note     = 7;
			package.velocity = 100;

	
			USB_MIDI_PackageSend_NonBlocking(package);
		}

		if(get_key_press(KEY_B))
		{
		
			midi_package_t package;

			package.type     = CC;
			package.event    = CC;
			package.note     = 7;
			package.velocity = 50;

	
			USB_MIDI_PackageSend_NonBlocking(package);
		}
			
		DELAY_Wait_uS(1000);
	}


}

