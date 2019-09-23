
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <time.h>
#include <sys/time.h>
#define CONSTSS 4
#define SPI_DELAY 400
#define SCL 1
#define SDI 2
static char answer;
int timeConst= 0;

void init_lcd(void);
void display1(const char *);
void display2(const char *);
void init_pwm(void);
void update_freq(int f);
void update_rgb(int r, int g, int b);

void init_keypad(void);
void scan_keypad(void);
void init_tim3(void);

void nano_wait(int );

int cond = 0;

extern void micro_wait(int);

//sound stuff
void dac_soft_trigger(void);
void solenoid_trig(void);

//declaration for sound functions
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




static int exitVar = 1;
static int exitVar1 = 1;
static int exitVar3 = 1;
static int temp6 = 0;
short temp[] = {
        0x002,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x0c0,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
};

static void sendbit(int b)
{
	GPIOC->BRR = SCL;
	GPIOC->BSRR = b ? SDI : (SDI << 16);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = SCL;
	nano_wait(SPI_DELAY);

}

static void sendbyte(char b)
{
	int var;
	for(var=8; var>0; var--)
	{
		sendbit(b & 0x80);
		b <<= 1;
	}
}


static void cmd(char b)
{
	GPIOC-> BRR = CONSTSS;
	nano_wait(SPI_DELAY);
	sendbit(0);
	sendbit(0);
	sendbyte(b);
	nano_wait(SPI_DELAY);
	nano_wait(SPI_DELAY);
}

static void data(char b)
{
	GPIOC->BRR = CONSTSS;
	nano_wait(SPI_DELAY);
	sendbit(1);
	sendbit(0);
	sendbyte(b);
	nano_wait(SPI_DELAY);
	GPIOC->BSRR = CONSTSS;
	nano_wait(SPI_DELAY);
}

void display1_dma(const char *s) {
	static short dispmem[] = {
	        0x002,
	        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
	        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
	        0x0c0,
	        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
	        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
	};
	//cmd(0x02); // put the cursor on the beginning of the first line.
		    int x;
		    for(x=0; x<16; x+=1)
		        if (s[x])
		        	dispmem[x+1] = s[x] | 0x200;
		        else
		            break;
		    for(   ; x<16; x+=1)
		    	dispmem[x] = 0x220;



RCC->AHBENR |= RCC_AHBENR_DMA1EN;
DMA1_Channel5->CCR &= ~DMA_CCR_EN;
DMA1_Channel5->CMAR = (uint32_t) dispmem;
DMA1_Channel5->CPAR = (uint32_t) &SPI2->DR;
DMA1_Channel5->CNDTR = 17;
SPI2->CR2 |= SPI_CR2_TXDMAEN;
DMA1_Channel5->CCR |= DMA_CCR_DIR;
DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;
DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0;
DMA1_Channel5->CCR |= DMA_CCR_MINC;
DMA1_Channel5->CCR &= ~DMA_CCR_PL;
//DMA1_Channel5->CCR |= DMA_CCR_CIRC;
DMA1_Channel5->CCR |= DMA_CCR_EN;


//for(int i=0;i <34;i++){
//	dispmem[i] = temp[i];
//}
}

void display1_spi(const char *s) {
	cmd(0x02); // put the cursor on the beginning of the first line.
	    int x;
	    for(x=0; x<16; x+=1)
	        if (s[x])
	            data(s[x]);
	        else
	            break;
	    for(   ; x<16; x+=1)
	        data(' ');



}

void init_lcd_spi(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	GPIOB->MODER |= 0x8A000000;
	//GPIOB->AFR[1] |= 0x10110000;


	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_BR;
	SPI2->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
	SPI2->CR2 = SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_DS_3 | SPI_CR2_DS_0;
	SPI2->CR1 |= SPI_CR1_SPE;

	nano_wait(100000000); // Give it 100ms to initialize
	cmd(0x38); // 0011 NF00 N=1, F=0: two lines
	cmd(0x0c); // 0000 1DCB: display on, no cursor, no blink
	cmd(0x01); // clear entire display
	nano_wait(6200000); // clear takes 6.2ms to complete
	cmd(0x02); // put the cursor in the home position
	cmd(0x06); // 0000 01IS: set display to increment
}

void init_lcd(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->ODR |= 7;
	GPIOC->MODER &= ~0x3f;
	GPIOC->MODER |= 0x15;

	nano_wait(100000000);
	cmd(0x38);
	cmd(0x0c);
	cmd(0x01);
	nano_wait(6200000);
	cmd(0x02);
	cmd(0x06);
}

void display1(const char *s)
{
	cmd(0x02);
	int length;
	for(length=0; length<16; length += 1)
	{
		if(s[length] == '\0')
			break;
		data(s[length]);
	}
}
void solenoid_trig(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~(3 << 2*10);
    GPIOA->MODER |= (1 << 2*10);
    GPIOA->ODR |= (1 << 10);
//    display2("it works fine ");

}

void display2(const char *s)
{
	cmd(0xc0);
	int length;
	for(length=0; length<16; length += 1)
	{
		if(s[length] == '\0')
			break;
		data(s[length]);
	}
}

void display1(const char*);
void display2(const char*);
void update_freq(int f);
void update_rgb(int r,int g, int b);
void nano_wait(int param);

//keypad row being examined

static int row = 0;

//================================
//Turn on a row at a time




void scan_keypad(void) {
				int x = 0;
				int y = 1;
				int z = 0;
		for(int i = 0;i < 300;i++){
			for(row=1; row<5; row++){
				GPIOC->BSRR = 1 <<(row+3);
				nano_wait(1000000);
				GPIOC->BRR = 0xf0;
				nano_wait(1000000);


//					if (exitVar1 == 0)
//					{
//						RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//					GPIOA->MODER |= 3<<(2*4);
//
//					RCC->APB1ENR |= RCC_APB1ENR_DACEN;
//					DAC->CR &= ~DAC_CR_EN1;
//					DAC->CR &= ~DAC_CR_BOFF1;
//					DAC->CR |= DAC_CR_TEN1;
//					DAC->CR |= DAC_CR_TSEL1;
//					DAC->CR |= DAC_CR_EN1;
//						while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
//												DAC->DHR12R1 = x;
//												DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
//												if(z == 0x0)
//												{
//													y = 1;
//												}
//												else if(z == 0x7fff)
//												{
//													y = 0;
//												}
//												z = (z+1)& 0xffff;
//												if(y == 1)
//												{
//													x = (x+40)&0xfff;
//												}
//												else{
//													x = 0;
//												}
//					}


			}

		}






}


//=======
//Handle button press

static void press(char c) {

	static int offset = 0;
//	static int read = 0;
	static int r,g,b;
	static int freq = 0;
	static char ledline[17];
	static char freqline[17];
	static char newest[17];
	static int entry = ' ';
	static int check = 0;
	temp6 = 1;
	if(c == answer)
	{
		TIM3->CR1 &= ~TIM_CR1_CEN;
//		return;
	}
//	TIM3->CR1 &= ~TIM_CR1_CEN;
	if (c == '*'){
		if(entry!= ' ') return;
		entry = '*';
		//strcpy(ledline, "LED: _");
//		display1(ledline);
		return;
	}

	if(c == '#'){
		if (entry != ' ') return;
		entry = '#';
		strcpy(freqline, "PASS: _");
		display2(freqline);
		return;
	}
	if (entry == '*'){

		main();
	}

	if (entry == '#'){
		newest[6+offset] = answer;
		freqline[6+offset] = c;
		if(offset<5)
			freqline[6+offset+1] = '_';
		if(newest[5+offset] == newest[6+offset])
		{
			if(newest[5+offset] == freqline[6+offset])
			{
				display1("INTRUDER!!");
				exitVar3 = 0;
				return;
			}
		}
		display2(freqline);
		freq = freq * 10 + (c - '0');
		if(c != answer){
			display2("Invalid password");
			check = 1;
			exitVar = 0;
			TIM3->CR1 &= ~TIM_CR1_CEN;
			dac_soft_trigger();
//			int x = 0; int y = 1; int z = 0;
//			RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//								GPIOA->MODER |= 3<<(2*4);
//
//								RCC->APB1ENR |= RCC_APB1ENR_DACEN;
//								DAC->CR &= ~DAC_CR_EN1;
//								DAC->CR &= ~DAC_CR_BOFF1;
//								DAC->CR |= DAC_CR_TEN1;
//								DAC->CR |= DAC_CR_TSEL1;
//								DAC->CR |= DAC_CR_EN1;
//									while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
//															DAC->DHR12R1 = x;
//															DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
//															if(z == 0x0)
//															{
//																y = 1;
//															}
//															else if(z == 0x7fff)
//															{
//																y = 0;
//															}
//															z = (z+1)& 0xffff;
//															if(y == 1)
//															{
//																x = (x+40)&0xfff;
//															}
//															else{
//																x = 0;
//															}
		}
		offset += 1;

		if(offset == 6){
			if(check != 1){
				freq=8;
				solenoid_trig();
				display2("Box unlocked  ");
			}
//			if (freq > 480000){
//				freq = 480000;
//				display2("FREQ: 480000");
//			}
			offset = 0;
			entry = ' ';
			update_freq(freq);
			freq=0;
		}
	}
}





//===============
//The Timer 2 ISR
//Determine the column from the timer channel
//Use teh current keypad roq to invoke a button press


void TIM2_IRQHandler() {
	if((TIM2->SR & TIM_SR_UIF) != 0){
		TIM2->SR &= ~TIM_SR_UIF;
		return;
	}



	//check if the channel 2 (column 1) is triggered
	if(TIM2->SR & TIM_SR_CC2IF){
		switch(row){
		case 1: press('1'); break;
		case 2: press('4'); break;
		case 3: press('7'); break;
		case 4: press('*'); break;
		}
	}

	//check if channel 3 (column 2) is triggered
	if(TIM2->SR & TIM_SR_CC3IF){
		switch(row){
		case 1: press('2'); break;
		case 2: press('5'); break;
		case 3: press('8'); break;
		case 4: press('0'); break;
		}
	}

	//check if channel 4 (column 3) is triggered.
	if(TIM2->SR & TIM_SR_CC4IF){
		switch (row){
		case 1: press('3'); break;
		case 2: press('6'); break;
		case 3: press('9'); break;
		case 4: press('#'); break;
		}
	}

	int val = TIM3->CCR2;																																					;
		srand( val + rand());
		srand( val + rand());
		srand( val + rand());


	//Wait for any remaining downward bounce to stop.
	nano_wait(10*1000*1000);

	//wait for the button to no longer be pressed

	while((GPIOA->IDR & 0xf) != 0)
		;

	//Wait for any remaining upward bounce to stop.
	nano_wait(10 * 1000 * 1000);

	//Reading CCRx clears CCxIF interrupt flag
	int __attribute((unused)) useless;
	useless = TIM2->CCR2;
	useless = TIM2->CCR3;
	useless = TIM2->CCR4;
	return;
}


void init_keypad(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOC->MODER |= 0X5500;
	GPIOC->BRR |= 0xf0;
	GPIOA->PUPDR &= ~(3<<2);
	GPIOA->PUPDR |= 2<<2;
	GPIOA->PUPDR |= 2<<4;
 	GPIOA->PUPDR |= 2<<6;
	GPIOA->MODER |= 0xA8;
	GPIOA->AFR[0] |= 0x2220;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	//channel 2
	TIM2->PSC = 0;
	TIM2->ARR = 0xffffffff;

	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;
	TIM2->CCMR1 &= ~TIM_CCMR1_IC2F;
	TIM2->CCMR1 &= ~TIM_CCMR1_IC2PSC;

	TIM2->CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP);
	TIM2->CCER |= TIM_CCER_CC2E;
	TIM2->DIER |= TIM_DIER_CC2IE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC->ISER[0] = 1<<TIM2_IRQn;

	TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;
	TIM2->CCMR1 |= TIM_CCMR1_IC1F_3 | TIM_CCMR1_IC1F_2 |
	TIM_CCMR1_IC1F_1 |TIM_CCMR1_IC1F_0;

	TIM2->CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;
	//channel 3

	TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;
	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
	TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;

	TIM2->CCER &= ~(TIM_CCER_CC3P|TIM_CCER_CC3NP);
	TIM2->CCER |= TIM_CCER_CC3E;
	TIM2->DIER |= TIM_DIER_CC3IE;


	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;
 	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 |
	TIM_CCMR2_IC3F_1 |TIM_CCMR2_IC3F_0;

	TIM2->CCMR2 |= TIM_CCMR2_IC3F_3 | TIM_CCMR2_IC3F_2 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC3F_0;


	//channel 4

	TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
	TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
	TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;

	TIM2->CCER &= ~(TIM_CCER_CC4P|TIM_CCER_CC4NP);
	TIM2->CCER |= TIM_CCER_CC4E;
	TIM2->DIER |= TIM_DIER_CC4IE;

	TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
	TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 |
	TIM_CCMR2_IC4F_1 |TIM_CCMR2_IC4F_0;


	TIM2->CCMR2 |= TIM_CCMR2_IC4F_3 | TIM_CCMR2_IC4F_2 | TIM_CCMR2_IC4F_1 | TIM_CCMR2_IC4F_0;

}
void init_tim3(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 48000 - 1;
	TIM3->ARR = 6000 - 1;
//	TIM3->CCR1 = 5000;
	TIM3->CR1 &= ~TIM_CR1_DIR;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM3->DIER |= TIM_DIER_UIE;
//	TIM3->DIER |= TIM_DIER_CC1IE;
	NVIC->ISER[0] = 1<<TIM3_IRQn;



}

//int _gettimeofday( struct timeval *tv, void *tzvp )
//{
//    uint64_t t = __your_system_time_function_here__();  // get uptime in nanoseconds
//    tv->tv_sec = t / 1000000000;  // convert to seconds
//    tv->tv_usec = ( t % 1000000000 ) / 1000;  // get remaining microseconds
//    return 0;  // return non-zero for error
//} // end _gettimeofday()
void TIM3_IRQHandler()
{
	int val = TIM3->CCR1;
	int val1 = TIM3->CCR2;
	int val2 = TIM3->CCR3;
	srand( val + rand());
	srand( val1 + rand());
	srand( val2 + rand());
//	timeConst++;
//	srand(timeConst);
	TIM3->SR &= ~TIM_SR_UIF;
//	if(TIM3->SR & TIM_SR_CC1IF)
//	{
//		display2("DONT BE SMART      ");
//		TIM3->SR &= ~TIM_SR_CC1IF;
//	}
//	exitVar1 = 0;




}




//char* concat(const char *string1, const char *string2)
//{
//    char *output = malloc(strlen(string1) + strlen(string2) + 1);
//    strcpy(output, string1);
//    strcat(output, string2);
//
//    return output;
//}

int main(void)
{
	//dac_soft_trigger();
	//for(;;);

	init_tim3();
	init_lcd();
	int x;
	display2("PASS: ******");

	char * input3 = "You have 3 seconds, ANSWER QUICK!!     ";
	int length = strlen(input3);
				for(x=0; x<=length+1; x+=1) {
					display1(&input3[x]);
					nano_wait(200000000);
					}

	int rand_num;
	char * input2;

//	srand(timeConst);
//		init_lcd_spi();
		//char * input1;
		//int digit_num = rand() % 6;

	for (int i = 0; i < 6; i++){

		if (i == 0){
//			srand(timeConst);
			rand_num = rand() % 10;
			if(rand_num == 0){input2 = "        First Digit: SQRT(64) / 2   "; answer = '4';}
			if(rand_num == 1){input2 = "        First Digit: SQRT(63) / 2   "; answer = '4';}
			if(rand_num == 2){input2 = "        First Digit: SQRT(62) / 2   "; answer = '4';}
			if(rand_num == 3){input2 = "        First Digit: SQRT(64) / 2   "; answer = '4';}
			if(rand_num == 4){input2 = "        First Digit: SQRT(60) / 2   "; answer = '4';}
			if(rand_num == 5){input2 = "        First Digit: SQRT(59) / 2   "; answer = '4';}
			if(rand_num == 6){input2 = "        First Digit: SQRT(58) / 2   "; answer = '4';}
			if(rand_num == 7){input2 = "        First Digit: SQRT(49) + 2   "; answer = '9';}
			if(rand_num == 8){input2 = "        First Digit: SQRT(56) / 2   "; answer = '4';}
			if(rand_num == 9){input2 = "        First Digit: SQRT(55) / 2   "; answer = '4';}

		}

		if (i == 1){
			rand_num = rand() % 10;
			//char * input2;
			if(rand_num == 0){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 1){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 2){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 3){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 4){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 5){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 6){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 7){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 8){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
			if(rand_num == 9){input2 = "        Second Digit: SQRT(64) / 4   "; answer = '2';}
		}


		if (i == 2){
			rand_num = rand() % 10;
			//char * input2;
			if(rand_num == 0){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 1){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 2){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 3){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 4){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 5){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 6){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 7){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 8){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
			if(rand_num == 9){input2 = "        Third Digit: SQRT(256) / 2   "; answer = '8';}
		}


		if (i == 3){
			rand_num = rand() % 10;
			//char * input2;
			if(rand_num == 0){input2 = "        Fourth Digit: ((1|0)&1)      "; answer = '1';}
			if(rand_num == 1){input2 = "   Lowest Common Multiple of 2 and 6 "; answer = '6';}
			if(rand_num == 2){input2 = "       Solution of x+24 = 31         "; answer = '7';}
			if(rand_num == 3){input2 = "       Solution of x+24 = 31         "; answer = '7';}
			if(rand_num == 4){input2 = "       Solution of x+24 = 31         "; answer = '7';}
			if(rand_num == 5){input2 = "  Lowest Common Multiple of 2 and 6  "; answer = '6';}
			if(rand_num == 6){input2 = "  Lowest Common Multiple of 2 and 6  "; answer = '6';}
			if(rand_num == 7){input2 = "  Lowest Common Multiple of 2 and 6  "; answer = '6';}
			if(rand_num == 8){input2 = "       Fourth Digit: ((1|0)&1)       "; answer = '1';}
			if(rand_num == 9){input2 = "       Fourth Digit: ((1|0)&1)       "; answer = '1';}

		}

		if (i == 4){
			rand_num = rand() % 10;
			//char * input2;
			if(rand_num == 0){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 1){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 2){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 3){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 4){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 5){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 6){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 7){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 8){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
			if(rand_num == 9){input2 = "        Fifth Digit: SQRT(64) / 2    "; answer = '4';}
		}



		if (i == 5){
			rand_num = rand() % 10;
			//char * input2;
			if(rand_num == 0){input2 = "          Sixth Digit: (120/2)/10    "; answer = '6';}
			if(rand_num == 1){input2 = "          Sixth Digit: (120/2)/10    "; answer = '6';}
			if(rand_num == 2){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}
			if(rand_num == 3){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}
			if(rand_num == 4){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}
			if(rand_num == 5){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}
			if(rand_num == 6){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}
			if(rand_num == 7){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}
			if(rand_num == 8){input2 = "          Sixth Digit: (120/2)/10    "; answer = '6';}
			if(rand_num == 9){input2 = "          Sixth Digit: (120/2)/10     "; answer = '6';}

		}

		int x;
//			char input[strlen(input1) + strlen(input2)];
//			strcat(input, input1);
//			strcat(input, input2);
			int length = strlen(input2);
			for(x=0; x<=length+1; x+=1) {
				display1(&input2[x]);
				nano_wait(200000000);
				}


			TIM3->CR1 |= TIM_CR1_CEN;
		init_keypad();

		init_pwm();
		update_rgb(1,1,1);
		update_freq(8);

	//    display1("LED: 010101");

//		if(exitVar == 0){
//			return 0;
//		}

		int temp4;
		scan_keypad();
		//dac_soft_trigger();
		temp4 = TIM_SR_UIF;
		if(exitVar == 1 && temp6 == 1){
			temp4 = ~TIM_SR_UIF;
				}
		temp6 = 0;
		if(temp4 == TIM_SR_UIF && exitVar != 0){
//		if(TIM3->SR && TIM_SR_UIF){
				exitVar1 = 0;
				display2("TIME OUT       ");
				 dac_soft_trigger();
//				int x = 0; int y = 1; int z = 0;
//				RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//				GPIOA->MODER |= 3<<(2*4);
//
//				RCC->APB1ENR |= RCC_APB1ENR_DACEN;
//				DAC->CR &= ~DAC_CR_EN1;
//				DAC->CR &= ~DAC_CR_BOFF1;
//				DAC->CR |= DAC_CR_TEN1;
//				DAC->CR |= DAC_CR_TSEL1;
//				DAC->CR |= DAC_CR_EN1;
//				while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
//					DAC->DHR12R1 = x;
//					DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
//					if(z == 0x0)
//					{
//						y = 1;
//					}
//					else if(z == 0x7fff)
//					{
//						y = 0;
//					}
//						z = (z+1)& 0xffff;
//					if(y == 1)
//					{
//						x = (x+40)&0xfff;
//					}
//					else{
//						x = 0;
//				}
		}
		if(exitVar == 0){


			return 0;

		}

		if(exitVar1 == 0){
			return 0;
		}
        if(exitVar3 == 0){
        	return 0;
        }
	}
	solenoid_trig();



}



































