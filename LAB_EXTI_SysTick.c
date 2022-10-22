/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2022-10-22 by HEODABIN
* @brief   Embedded Controller:  LAB_Systick
*
******************************************************************************
*/


#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecSysTick.h"
#include "ecEXTI.h"

#define LED_PIN 	5
#define BUTTON_PIN 13

int SEVENSEGMENT_NUM = 0;
void setup(void);

int main(void) {
	// System CLOCK, Systick, sevensegment Initialiization ----------------------------------------
	setup();
	
	while (1){
		sevensegment_decode(SEVENSEGMENT_NUM%10);
		SEVENSEGMENT_NUM++;
		delay_ms(1000);
	}
}

void EXTI15_10_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {
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
	// Initialize GPIOC_13 for Input Button
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
	GPIO_pupd(GPIOC, BUTTON_PIN, EC_PU);
	EXTI_init(GPIOC, BUTTON_PIN, FALL, 2);
}
