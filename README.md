# LAB_EXTI-SysTick
### LAB : EXTI_SysTick

**Date:** 2022.10.22

**Author/Partner:** Heo Dabin/Ga Seungho

**Github:** [Github code](https://github.com/DABINHEO/LAB_EXTI-SysTick.git)

**Demo Video:** [Youtube Link(EXTI)](https://youtu.be/ov-GdQ-njmE), [Youtube Link(Systick)](https://youtu.be/17MWPSjd88M)

##            



### Introduction

In this LAB, four LED toggling using button and counting of 7segment display over time will be performed. Button is an external interrupt, which will interrupt the main code, execute another code, and then return to the main code. For the above description, we wrote a header code that makes button available as an External Interrupt. The seven-segment display will automatically count from 0 to 9 and returns 0 again with a delay of 1 second per number. And pressing the button returns the counter to zero again. We wrote a header code for this delay.



### Requirement

#### Hardware

* MCU

  * NUCLEO-F401RE

* Actuator/Display

  * LED x 4 
  * 7-segment display(5101ASR)

* Others

  * Resistor 330 ohm x 4
  * breadboard

  

#### Software

* Keil uVision, CMSIS, EC_HAL library

##          



### Problem1: LED Toggle with EXTI Button



#### Procedure

In this problem, the built-in button of NUCLEO-F401RE will be used as an external interrupt, and the four LEDs will be lit one by one, LED0 -> LED1 -> LED2 -> LED3 -> LED0. So, first of all, the button was set up to use as EXTI. 

So, first of all, I set it up to use button as EXTI. First, the system clock was enabled, and the pin of the GPIO was placed in the right place. (Pins 1 to 4 are assigned to SYSCFG_EXTICR1, numbers 5 to 8 are assigned to SYSCFG_EXTICR2, numbers 9 to 12 are assigned to SYSCFG_EXTICR3, and numbers 13 to 16 are assigned to SYSCFG_EXTICR4. In addition, we can set which GPIO to use by modifying the register.) Next, we choose whether to use the falling trigger or the rising trigger, and masking was set so that we can use the interrupt. Next, we decided what number to do this interrupt and set NVIC through the built-in function.



#### Configuration

![image](https://user-images.githubusercontent.com/113574284/197318338-7aa0e376-db2a-4f75-a84c-8095f2d5b951.png)



#### Circuit Diagram

![image](https://user-images.githubusercontent.com/113574284/197318879-33d3a159-ab88-4678-b15d-4cd660de11dd.png)



#### Discussion

1. To detect an external signal we can use two different methods: polling and interrupt. What are the advantages and disadvantages of each approach?

   Polling periodically checks whether or not you have received a signal to perform an event, and interrupt is a method of executing an event through a handler when you receive a signal to perform an event.

   Interrupt's strengths include accurate timing and short reaction time, but it is disadvantageous in implementation because an event needs to pass through a handler. Polling has the advantage of being easy to implement, but it has the disadvantage of slow reaction time and taking up a lot of resources.

   

2. What would happen if the EXTI interrupt handler does not clear the interrupt pending flags? Check with your code

   The first interrupt works normally, but it does not work from the second.

* ecEXTI.c  description

```c++
//EXTI setting
void EXTI_init(GPIO_TypeDef *Port, int Pin, int trig_type,int priority){

	// SYSCFG peripheral clock enable	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		
	
	// Connect External Line to the GPIO
	int EXTICR_port;
	if		(Port == GPIOA) EXTICR_port = 0;
	else if	(Port == GPIOB) EXTICR_port = 1;
	else if	(Port == GPIOC) EXTICR_port = 2;
	else if	(Port == GPIOD) EXTICR_port = 3;
    else 					EXTICR_port = 4;
	
	SYSCFG->EXTICR[Pin/4] &= ~15<<(Pin%4)*4;			// clear 4 bits
	SYSCFG->EXTICR[Pin/4] |= EXTICR_port<<(Pin%4)*4;	// set 4 bits
	
	// Configure Trigger edge
	if (trig_type == FALL) 		EXTI->FTSR |= 1<<Pin;  	 // Falling trigger enable 
	else if	(trig_type == RISE) EXTI->RTSR |= 1<<Pin;    // Rising trigger enable 
	else if	(trig_type == BOTH) {						 // Both falling/rising trigger enable
		EXTI->RTSR |= 1<<Pin; 
		EXTI->FTSR |= 1<<Pin;
	} 
	
	// Configure Interrupt Mask (Interrupt enabled)
	EXTI->IMR |= 1<<Pin;     // not masked
	
	
	// NVIC(IRQ) Setting
	int EXTI_IRQn = 0;
	
	if (Pin < 5) 		EXTI_IRQn = Pin + 6;
	else if	(Pin < 10) 	EXTI_IRQn = EXTI9_5_IRQn;
	else 				EXTI_IRQn = EXTI15_10_IRQn;
								
	NVIC_SetPriority(EXTI_IRQn, 0);	// EXTI priority
	NVIC_EnableIRQ(EXTI_IRQn); 		// EXTI IRQ enable
}


void EXTI_enable(uint32_t pin) {
	EXTI->IMR = 1<<pin;     	// not masked (i.e., Interrupt enabled)
}
void EXTI_disable(uint32_t pin) {
	EXTI->IMR &= ~(1<<pin);     // masked (i.e., Interrupt disabled)
}

uint32_t is_pending_EXTI(uint32_t pin){
	uint32_t EXTI_PRx = (EXTI->PR >> pin) & 1;     	// check  EXTI pending 	
	return EXTI_PRx;
}


void clear_pending_EXTI(uint32_t pin){
	EXTI->PR |= 1 << pin;     // clear EXTI pending 
}
```



* LAB_EXTI.c  description

```c++
// Global variable
int EXTI_LED_NUM;

int main(void) {
	int *count = 0;
	setup(); 	// System CLOCK, GPIO, EXTI Initialiization
    while (1);
}

void EXTI15_10_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {		
		led_decode(EXTI_LED_NUM%4);			//toggle LED
		EXTI_LED_NUM++;
		clear_pending_EXTI(BUTTON_PIN); 	// cleared by writing '1'
		for(int i = 0; i < 5000000; i++){}	// delay
	}
}

// Initialiization 
void setup(void)
{
	RCC_PLL_init();                         // System Clock = 84MHz
	// Initialize GPIOA_5 for Output
	LED_init();
	// Initialize GPIOC_13 for Input Button, EXTI
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
	GPIO_pupd(GPIOC, BUTTON_PIN, EC_PU);
	EXTI_init(GPIOC, BUTTON_PIN, FALL, 2);
}
```



#### Results

![image](https://user-images.githubusercontent.com/113574284/197322959-4853bbaf-9608-4a22-b313-65e5b85456a9.png)

[Youtube Link(EXTI)](https://youtu.be/ov-GdQ-njmE)

##          



### Problem2: Counting number on 7-Segment



#### Procedure

In this problem, we will use Systic to create a seven-segment display that counts from 0 to 9 at 1-second intervals and then returns to 0 to repeat counting to 9. According to the Internet, Systick can be use only in privileged mode and it is a 'core peripheral' so you do not need enable other clock or peripheral.(usefull for low power) It is a native autoreload counter so it do not need the tipical init sequence to force a general purpose timer in autoreload mode. I suggest you to use it, if your application run without a RTOS or scheduler.
For EXTI, a button will be used and pressing the button will cause the counting to return to zero. Therefore, we first set up the Systic. First, we set SysTick IRQ and SysTick Counter to disable. And since the PLL is 84 kHz, set the appropriate value to make 0.1ms. And set the initial value, and then enable the SysTick IRQ and SysTick Counter again.

#### Configuration

![image](https://user-images.githubusercontent.com/113574284/197324243-2940406e-a22e-4071-9d4d-940d396c1785.png)



#### Circuit Diagram

![image](https://user-images.githubusercontent.com/113574284/197325066-fe8eec1a-e3c4-4f2a-8334-d66fda2f50de.png)



#### Description with Code

* ecSysTic.c  description

```c++
volatile uint32_t msTicks;

void SysTick_init(void){	
	//  SysTick Control and Status Register
	SysTick->CTRL = 0;									// Disable SysTick IRQ and SysTick Counter

	// Select processor clock
	// 1 = processor clock;  0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// uint32_t MCU_CLK=EC_SYSTEM_CLK
	// SysTick Reload Value Register
	SysTick->LOAD = 84000000 / 1000 - 1;				// 1ms, for HSI PLL = 84MHz.

	// SysTick Current Value Register
	SysTick->VAL = 0;

	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		
	NVIC_SetPriority(SysTick_IRQn, 16);		// Set Priority to 1
	NVIC_EnableIRQ(SysTick_IRQn);			// Enable interrupt in NVIC
}

void SysTick_Handler(void){
	SysTick_counter();	
}

void SysTick_counter(){
	msTicks++;
}	

void delay_ms (uint32_t mesc){
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < mesc);
	
	msTicks = 0;
}

void SysTick_reset(void)
{
	// SysTick Current Value Register
	SysTick->VAL = 0;
}

uint32_t SysTick_val(void) {
	return SysTick->VAL;
}
```

* LAB_EXTI_SysTick.c  description

```c++
int SEVENSEGMENT_NUM = 0;
void setup(void);

int main(void) {
	// System CLOCK, Systick, sevensegment Initialiization ----------------------------------------
	setup();
	
    // sevensegment counting display
	while (1){
		sevensegment_decode(SEVENSEGMENT_NUM%10);
		SEVENSEGMENT_NUM++;
		delay_ms(1000);
	}
}

// Button Interrupt
void EXTI15_10_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {
        // Counting return 0
		SEVENSEGMENT_NUM = 0;
		clear_pending_EXTI(BUTTON_PIN); 		// cleared by writing '1'
	}
}

// Initialiization 
void setup(void)
{
	RCC_PLL_init();                         // System Clock = 84MHz
	// Initialize GPIO for sevensegment
	sevensegment_init();
	// Initialize Systick
	SysTick_init();
	// Initialize GPIOC_13 for Input Button, EXTI
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
	GPIO_pupd(GPIOC, BUTTON_PIN, EC_PU);
	EXTI_init(GPIOC, BUTTON_PIN, FALL, 2);
}
```



#### Results

![image](https://user-images.githubusercontent.com/113574284/197325763-954b7553-4289-46b3-8e07-dfa5e66e905e.png)

[Youtube Link](https://youtu.be/17MWPSjd88M)



##          



### Reference

[LAB: EXTI & SysTick](https://ykkim.gitbook.io/ec/course/lab/lab-exti-and-systick#problem-2-counting-number-on-7-segment)

Class materials in Embedded Controller by Prof. Kim

[NAVER blog](https://blog.naver.com/PostView.nhn?isHttpsRedirect=true&blogId=bluelife85&logNo=220712311636)

RM0383 Reference manual

##          



### Troubleshooting

When toggling four LEDs, I tried to toggle using conditional statements by reading GPIO->ODD, but it didn't work well and I found out that the light was not 1.
I made a toggle using conditional statements, but I thought it was inefficient, and I tried to use a pointer to use the variable declared in the main function, but it failed. As a result of the investigation, the problem could be solved by finding global variables.
