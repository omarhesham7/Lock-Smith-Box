//#include "stm32f0xx.h"
//#include "stm32f0_discovery.h"
//#include <string.h>
//#include "poop.h"
//
//void display1(const char*);
//void display2(const char*);
//void update_freq(int f);
//void update_rgb(int r,int g, int b);
//void nano_wait(int param);
//
////keypad row being examined
//
//static int row = 0;
//
////================================
////Turn on a row at a time
//
//
//
//
//void scan_keypad(void) {
//		for(int i = 0;i < 300;i++)
//			for(row=1; row<5; row++){
//				GPIOA->BSRR = 1 <<(row+3);
//				nano_wait(1000000);
//				GPIOA->BRR = 0xf0;
//				nano_wait(1000000);
//			}
//}
//
//
////=======
////Handle button press
//
//static void press(char c) {
//	static int offset = 0;
//	static int r,g,b;
//	static int freq = 0;
//	static char ledline[17];
//	static char freqline[17];
//	static int entry = ' ';
//	if (c == '*'){
//		if(entry!= ' ') return;
//		entry = '*';
//		//strcpy(ledline, "LED: _");
//		display1(ledline);
//		return;
//	}
//	if(c == '#'){
//		if (entry != ' ') return;
//		entry = '#';
//		strcpy(freqline, "FREQ: _");
//		display2(freqline);
//		return;
//	}
//	if (entry == '*'){
//		ledline[5+offset]=c;
//		if(offset < 5)
//			ledline[5+offset+1] = '_';
//		display1(ledline);
//		switch(offset){
//		case 0: r = 10* (c-'0'); offset++; break;
//		case 1: r += (c - '0'); offset++; break;
//		case 2: g = 10 * (c-'0'); offset++;break;
//		case 3: g+= (c - '0'); offset++; break;
//		case 4: b = 10* (c-'0'); offset++; break;
//		case 5: b+= (c - '0'); offset=0; update_rgb(r,g,b); entry=' '; break;
//		}
//	}
//
//	if (entry == '#'){
//		freqline[6+offset] = c;
//		if(offset<5)
//			freqline[6+offset+1] = '_';
//		display2(freqline);
//		freq = freq * 10 + (c - '0');
//		if(c != answer){
//			display2("WRONG BITCH");
//		}
//		offset += 1;
//		if(offset == 6){
//			if(freq<8){
//				freq=8;
//				display2("FREQ: 000008");
//			}
//			if (freq > 480000){
//				freq = 480000;
//				display2("FREQ: 480000");
//			}
//			offset = 0;
//			entry = ' ';
//			update_freq(freq);
//			freq=0;
//		}
//	}
//}
//
//
//
//
//
////===============
////The Timer 2 ISR
////Determine the column from the timer channel
////Use teh current keypad roq to invoke a button press
//
//
//void TIM2_IRQHandler() {
//	if((TIM2->SR & TIM_SR_UIF) != 0){
//		TIM2->SR &= ~TIM_SR_UIF;
//		return;
//	}
//
//
//
//	//check if the channel 2 (column 1) is triggered
//	if(TIM2->SR & TIM_SR_CC2IF){
//		switch(row){
//		case 1: press('1'); break;
//		case 2: press('4'); break;
//		case 3: press('7'); break;
//		case 4: press('*'); break;
//		}
//	}
//
//	//check if channel 3 (column 2) is triggered
//	if(TIM2->SR & TIM_SR_CC3IF){
//		switch(row){
//		case 1: press('2'); break;
//		case 2: press('5'); break;
//		case 3: press('8'); break;
//		case 4: press('0'); break;
//		}
//	}
//
//	//check if channel 4 (column 3) is triggered.
//	if(TIM2->SR & TIM_SR_CC4IF){
//		switch (row){
//		case 1: press('3'); break;
//		case 2: press('6'); break;
//		case 3: press('9'); break;
//		case 4: press('#'); break;
//		}
//	}
//
//
//
//
//	//Wait for any remaining downward bounce to stop.
//	nano_wait(10*1000*1000);
//
//	//wait for the button to no longer be pressed
//
//	while((GPIOA->IDR & 0xf) != 0)
//		;
//
//	//Wait for any remaining upward bounce to stop.
//	nano_wait(10 * 1000 * 1000);
//
//	//Reading CCRx clears CCxIF interrupt flag
//	int __attribute((unused)) useless;
//	useless = TIM2->CCR2;
//	useless = TIM2->CCR3;
//	useless = TIM2->CCR4;
//	return;
//}
//
//
//void init_keypad(void) {
////	//your code goes here
////
////	//initialize the PA4, PA5, PA6, PA7 and set outputs to zero
////
////	//to initialize as output set the pins to values 01
////
////
////
////
////	//This line of code used to enable port a and port c
////	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
////
////
////
////	//for lab7, we need gpioa
////	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
////
////	//GPIOC->MODER &= 0x000fffff;
////	GPIOA->MODER |= 0X5500;
////	//GPIOC->MODER |= 0x00055555;
////	//GPIOA->BRR |= 0xf0;					// <--- Confirm with gta if we really need to use the brr.
////
////	GPIOA->PUPDR &= ~(3<<2);
////	GPIOA->PUPDR |= 2<<2;
////	GPIOA->MODER &= ~(3<<2);
////	GPIOA->MODER |= 2<<(2*1);
////	GPIOA->AFR[0] &= ~(0xf<<(4*1));
////	GPIOA->AFR[0] |= 0x2<<(4*1);
////	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
////
////
////	//initialize channel 2
////	TIM2->PSC = 0;						//try and see if you can come back for edits to the prescaler
////	TIM2->ARR = 0xffffffff;
////
////	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
////	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
////	TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
////	TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;
////
////	TIM2->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP);
////	TIM2->CCER |= TIM_CCER_CC2E;
////	TIM2->DIER |= TIM_DIER_CC2IE;
////	TIM2->CR1 |= TIM_CR1_CEN;
////	NVIC->ISER[0] = 1<<TIM2_IRQn;
////
////
////	TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
////	TIM2->CCMR1 |= TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2 |
////	TIM_CCMR1_IC1F_1 |TIM_CCMR1_IC1F_0;
////
////	TIM2->CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;
////
////	//channel 3
////
////	TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
////	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
////	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
//// 	TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;
////
////	TIM2->CCER &= ~(TIM_CCER_CC3P|TIM_CCER_CC3NP);
////	TIM2->CCER |= TIM_CCER_CC3E;
////	TIM2->DIER |= TIM_DIER_CC3IE;
////		 	 	//TIM2->CR1 |= TIM_CR1_CEN;
////		 	 	//NVIC->ISER[0] = 1<<TIM2_IRQn;
////
////	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
////	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 |
////	TIM_CCMR2_IC3F_1 |TIM_CCMR2_IC3F_0;
////
//// 	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;
////
////
//// 	//channel 4
////
////	 TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
////	 TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
////	 TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
////	 TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;
////
////	 TIM2->CCER &= ~(TIM_CCER_CC4P|TIM_CCER_CC4NP);//unsure
////	 TIM2->CCER |= TIM_CCER_CC4E;
////	 TIM2->DIER |= TIM_DIER_CC4IE;
////	 //TIM2->CR1 |= TIM_CR1_CEN;
////	 //NVIC->ISER[0] = 1<<TIM2_IRQn;
////
////	 TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
////	 TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 |
////	 TIM_CCMR2_IC4F_1 |TIM_CCMR2_IC4F_0;
////
////	 TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 | TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;
////
////
////
////
////
////
////
//
//
////
////	for(;;)
////	asm("wfi");
//
//
//
//
//
//	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//	GPIOA->MODER |= 0X5500;
//	GPIOA->BRR |= 0xf0;
//	GPIOA->PUPDR &= ~(3<<2);
//	GPIOA->PUPDR |= 2<<2;
//	GPIOA->PUPDR |= 2<<4;
// 	GPIOA->PUPDR |= 2<<6;
//	GPIOA->MODER |= 0xA8;
//	GPIOA->AFR[0] |= 0x2220;
//	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
//
//	//channel 2
//	TIM2->PSC = 0;
//	TIM2->ARR = 0xffffffff;
//
//	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
//	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
//	TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
//	TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;
//
//	TIM2->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP);
//	TIM2->CCER |= TIM_CCER_CC2E;
//	TIM2->DIER |= TIM_DIER_CC2IE;
//	TIM2->CR1 |= TIM_CR1_CEN;
//	NVIC->ISER[0] = 1<<TIM2_IRQn;
//
//	TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
//	TIM2->CCMR1 |= TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2 |
//	TIM_CCMR1_IC1F_1 |TIM_CCMR1_IC1F_0;
//
//	TIM2->CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;
//	//channel 3
//
//	TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
//	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;
//
//	TIM2->CCER &= ~(TIM_CCER_CC3P|TIM_CCER_CC3NP);
//	TIM2->CCER |= TIM_CCER_CC3E;
//	TIM2->DIER |= TIM_DIER_CC3IE;
//
//
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
// 	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 |
//	TIM_CCMR2_IC3F_1 |TIM_CCMR2_IC3F_0;
//
//	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;
//
//
//	//channel 4
//
//	TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
//	TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;
//
//	TIM2->CCER &= ~(TIM_CCER_CC4P|TIM_CCER_CC4NP);//unsure
//	TIM2->CCER |= TIM_CCER_CC4E;
//	TIM2->DIER |= TIM_DIER_CC4IE;
//
//	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
//	TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 |
//	TIM_CCMR2_IC4F_1 |TIM_CCMR2_IC4F_0;
//
//
//	TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 | TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
