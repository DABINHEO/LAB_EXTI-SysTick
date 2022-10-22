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

#define LED_PIN 	5
#define BUTTON_PIN 13

void setup(void);

int main(void) {
	// System CLOCK, Systick, sevensegment Initialiization ----------------------------------------
	setup();
	int num = 0;

	while (1){
		sevensegment_decode(num%10);
		num++;
		delay_ms(1000);
	}
}

// Initialiization 
void setup(void)
{
	RCC_PLL_init();                         // System Clock = 84MHz
	sevensegment_init();
	SysTick_init();
}

