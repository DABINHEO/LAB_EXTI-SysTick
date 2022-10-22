/**
******************************************************************************
* @author  SSSLAB
* @Mod		 2021-8-12 by YKKIM
* @brief   Embedded Controller:  Tutorial ___
*					 - _________________________________
*
******************************************************************************
*/




#include "ecRCC.h"
#include "ecGPIO.h"
#include "ecEXTI.h"

#define LED_PIN 	5
#define BUTTON_PIN 13

int EXTI_LED_NUM;

void setup(void);



int main(void) {
	int *count = 0;
	// System CLOCK, GPIO Initialiization ----------------------------------------
	setup();
	

	// EXTI Initialiization ------------------------------------------------------	
	
	
	while (1);
}


void EXTI15_10_IRQHandler(void) {
	if (is_pending_EXTI(BUTTON_PIN)) {
		led_decode(EXTI_LED_NUM%4);
		EXTI_LED_NUM++;
		clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
		for(int i = 0; i < 5000000; i++){}
	}
}


// Initialiization 
void setup(void)
{
	RCC_PLL_init();                         // System Clock = 84MHz
	// Initialize GPIOA_5 for Output
	LED_init();
	// Initialize GPIOC_13 for Input Button
	GPIO_init(GPIOC, BUTTON_PIN, INPUT);  // calls RCC_GPIOC_enable()
	GPIO_pupd(GPIOC, BUTTON_PIN, EC_PU);
	EXTI_init(GPIOC, BUTTON_PIN, FALL, 2);

}

