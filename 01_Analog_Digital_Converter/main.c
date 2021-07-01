#define CORE_CM4
#include "usart.h"

#define BTN_pin					( 1U << 13 )
#define LED_pin					( 1U << 14)
int counter = 0;
char key;

int main(void)
{
	/*Enabling USART3*/
	USART3_rxtx_init();
	/*Initializing BUTTON*/
	BTN_init();
	/*Initializing LED*/
	LED_init();

	while(1)
	{
		/*Check if Blue button is pressed*/
		if (GPIOC -> IDR & BTN_pin)
		{
			/*Increment counter*/
			counter += 1;
			/*Print to console*/
			printf("%d reizes!\r\n", counter);
		}

		/*Constantly reading keyboard for pressed key*/
		//key = USART3_READ();

		/*If "1" is pressed*/
		//if (key == '1')
		//{
			//Turn on LED
			//GPIOB -> ODR |= LED_pin;
		//}
		//else
		//{
			//Turn off LED
			//GPIOB -> ODR &= ~LED_pin;
		//}
	}
}
