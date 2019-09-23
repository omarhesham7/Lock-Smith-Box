#ifndef TEMP_H
#define TEMP_H



#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "sinewave.h"
//#include "temp.h"
//void dac_soft_trigger(void);

void dac_soft_trigger(void);

void dac_soft_trigger(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= 3<<(2*4);

	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	DAC->CR &= ~DAC_CR_EN1;
	DAC->CR &= ~DAC_CR_BOFF1;
	DAC->CR |= DAC_CR_TEN1;
	DAC->CR |= DAC_CR_TSEL1;
	DAC->CR |= DAC_CR_EN1;

	int x = 0;
	int y = 1;
	int z = 0;

	for(;;)
	{
		while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
		DAC->DHR12R1 = x;
		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
		if(z == 0x0)
		{
			y = 1;
		}
		else if(z == 0x7fff)
		{
			y = 0;
		}
		z = (z+1)& 0xffff;
		if(y == 1)
		{
			x = (x+40)&0xfff;
		}
		else{
			x = 0;
		}
	}
}

#endif

