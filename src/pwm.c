#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

void init_pwm(void) {

	//pattern excerpt from lecture slides

	//setup the gpioa to determine the mode in which the registers will be used
	GPIOA->MODER |= 0x2a0000;

	//next, choose the type of Alternate function that we'd be working with
	GPIOA->AFR[1] &= ~(0xf<<(4*0));
	GPIOA->AFR[1] |= 0x2<<(4*0);
	GPIOA->AFR[1] &= ~(0xf<<(4*1));
	GPIOA->AFR[1] |= 0x2<<(4*1);
	GPIOA->AFR[1] &= ~(0xf<<(4*2));
	GPIOA->AFR[1] |= 0x2<<(4*2);

	//initialize enable
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = 1000;
	TIM1->ARR = 100 - 1;
	//Double check

	//From the lecture, we need the initializations of the respective CCRs of Timer 1
	TIM1->CCR1 = 99;
	TIM1->CCR2 = 99;
	TIM1->CCR3 = 99;


	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
	TIM1->CCER |= TIM_CCER_CC1E;
	TIM1->CCER |= TIM_CCER_CC2E;
	TIM1->CCER |= TIM_CCER_CC3E;
	TIM1->BDTR |= TIM_BDTR_MOE;

	//last step, enable the CR1
	TIM1->CR1 |= TIM_CR1_CEN;



}

void update_freq(int freq) {
	//Start by initializing the enable    					// <-- Update, not necessary
	//RCC->APB2ENR = RCC_APB2ENR_TIM1EN;

	//use formula and solve for TIM1_PSC
		TIM1->PSC = (48000000/freq/100)-1;
}

void update_rgb(int r, int g, int b) {
   TIM1->CCR1 = r;
   TIM1->CCR2 = g;
   TIM1->CCR3 = b;
}
