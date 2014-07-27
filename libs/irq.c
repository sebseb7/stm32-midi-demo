#include "libs/irq.h"

static uint32_t nested_ctr;

// stored priority level before IRQ has been disabled (important for co-existence with vPortEnterCritical)
static uint32_t prev_primask;


void IRQ_Disable(void)
{
	// get current priority if nested level == 0
	if( !nested_ctr ) {
		__asm volatile (			   \
				"	mrs %0, primask\n" \
				: "=r" (prev_primask)  \
				);
	}

	// disable interrupts
	__asm volatile ( \
			"	mov r0, #1     \n" \
			"	msr primask, r0\n" \
			:::"r0"	 \
			);

	++nested_ctr;

}

int32_t IRQ_Enable(void)
{
	// check for nesting error
	if( nested_ctr == 0 )
		return -1; // nesting error

	// decrease nesting level
	--nested_ctr;

	// set back previous priority once nested level reached 0 again
	if( nested_ctr == 0 ) {
		__asm volatile ( \
				"	msr primask, %0\n" \
				:: "r" (prev_primask)  \
				);
	}

	return 0; // no error
}

int32_t IRQ_Install(uint8_t IRQn, uint8_t priority)
{
	// no check for IRQn as it's device dependent

	if( priority >= 16 )
		return -1; // invalid priority

	u32 tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700)) >> 8;
	u32 tmppre = (4 - tmppriority);
	tmppriority = priority << tmppre;
	tmppriority = tmppriority << 4;
	NVIC->IP[IRQn] = tmppriority;

	NVIC_EnableIRQ(IRQn);

	return 0; // no error
}


void IRQ_DeInstall(uint8_t IRQn)
{
	NVIC_DisableIRQ(IRQn);
}

