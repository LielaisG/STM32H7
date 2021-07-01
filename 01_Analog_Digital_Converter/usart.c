/*
 * usart.c
 *
 *  Created on: June 20, 2021
 *  Author: Gatis Fridenbergs
 */

/*
 * MAIN FUNCTION PROCEDURE
 * 1.
 * 2.
 * 3.
 * 4.
 * 5.
 * 6.
 * */

/*Defines & Includes*/
#define CORE_CM4
#include "usart.h"

/*Function initialization*/
static uint16_t CALC_USART_BD(uint32_t PeriphClk, uint32_t BaudRate);
static void USART_SET_BAUDRATE(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);

#define SYS_FREQ					64000000
#define APB1_CLK					SYS_FREQ
#define USART3_BAUDRATE				9600
#define ISR_TXE						( 1U << 7 )
#define ISR_RXNE					( 1U << 5 )
#define CR1_TE						( 1U << 3 )
#define CR1_RE						( 1U << 2 )
#define CR1_UE						( 1U << 0 )
#define USART3_ENR					( 1U << 18 )
#define GPIOD_ENR					( 1U << 3 )
#define GPIOC_ENR					( 1U << 2 )
#define GPIOB_ENR					( 1U << 1 )
#define PD8_16						( 1U << 16 )
#define PD8_17						( 1U << 17 )
#define PD9_18						( 1U << 18 )
#define PD9_19						( 1U << 19 )
#define PC13_26						( 1U << 26 )
#define PC13_27						( 1U << 27 )
#define PB14_28						( 1U << 28 )
#define PB14_29						( 1U << 29 )

/*Function declerations*/
void USART3_rxtx_init(void)
{
	/*GPIO PC3 @ ADC3_INP1*/

	/********** STEPS TO FOLLOW ***********
	 * 1. Enable USART and GPIO clock
	 * 2. Set the alternate mode for PD8 & PD9
	 * 3. Set the alternate mode type to AF7
	 * 4. Configuring baud rate
	 * 5. Transmitter & Receiver enable
	 * 6. Enabling USART3 module
	 * ***********************************/

	/*1. Enable USART and GPIO clock*/
	RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIODEN);								//Enable GPIOB & GPIOD clock
	RCC->APB1LENR |= RCC_APB1LENR_USART3EN;

	/*2. Set the alternate mode for PD8 & PD9*/
	GPIOD->MODER &= ~(GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0);									//Alternate function mode
	GPIOD->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);

	/*3. Set the alternate mode type to AF7*/

	GPIOD->AFR[1] |= (GPIO_AFRH_AFSEL8_0 | GPIO_AFRH_AFSEL8_1 | GPIO_AFRH_AFSEL8_2);			//0111: AF7 Alternate function selection
	GPIOD->AFR[1] |= (GPIO_AFRH_AFSEL9_0 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2);

	/*4. Configuring baud rate*/
	USART_SET_BAUDRATE(USART3, APB1_CLK, USART3_BAUDRATE);										//Set the baudrate

	//5. Transmitter & Receiver enable
	USART3->CR1 = (USART_CR1_TE | USART_CR1_TE);												//Transmitter & receiver enable

	/*6. Enabling USART3 module*/
	USART3->CR1 |= USART_CR1_UE;																//USART3 enable
}

void BTN_init(void)
{
	/********** STEPS TO FOLLOW ***********
	 * 1. Enable GPIO clock
	 * 2. Set PC13 as input
	 * ***********************************/

	/*1. Enable GPIO clock*/
	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;														//Enable GPIO clock

	/*2. Set PC13 as input*/
	GPIOC->MODER &= ~(GPIO_MODER_MODE13_0 | GPIO_MODER_MODE13_1);								//Set pin as input
}

void LED_init(void)
{
	GPIOB -> MODER &= ~PB14_29;
	GPIOB -> MODER |= PB14_28;
}

char USART3_READ(void)
{
	/*Make sure receive data register is not empty*/
	//While RXNE bit = 0, then wait
	while (!(USART3->ISR & ISR_RXNE)){}

	//Return received character
	return USART3 -> RDR;
}

void USART3_WR(int ch) {
	/*Make sure transmit data register is empty*/
	//While the TXE bit = 0 (DATA REGISTER FULL), THEN WAIT
	while (!(USART3->ISR & ISR_TXE)) {}

	//Write into transmit data register
	USART3->TDR = (ch & 0xFF);
}

/*USART BAUDRATE CALCULATION*/
static uint16_t CALC_USART_BD(uint32_t PeriphClk, uint32_t BaudRate) {
	//return ( ( PeriphClk + ( BaudRate / 2U ) ) / BaudRate );
	return (uint16_t) (PeriphClk / BaudRate);
}

/*SETTING THE BAUDRATE FOR USART3*/
static void USART_SET_BAUDRATE(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {
	USARTx->BRR = CALC_USART_BD(PeriphClk, BaudRate);
}
/*Print function*/
int __io_putchar(int ch)
{
	USART3_WR(ch);
	return ch;
}
