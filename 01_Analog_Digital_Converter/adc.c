/*
 * adc.c
 *
 *  Created on: June 28, 2021
 *  Author: Gatis Fridenbergs
 */

/*
 * MAIN FUNCTION PROCEDURE
 * 1. Define Var for storing ADC data
 * 2. Initiate ADC
 * 3. Launch offset calibration
 * 4. Enbale ADC
 * 5. Start conversion
 * 6. Save data in var for further processing
 * */

/*Defines & Includes*/
#define CORE_CM4
#include "adc.h"

/*Function declerations*/
void ADC_init(void)
{
	/*GPIO PC3 @ ADC3_INP1*/

	/********** STEPS TO FOLLOW ***********
	 * 1. Enable ADC and GPIO clock
	 * 2. Set the prescaler and clock mode in CCR
	 * 3. Set the resolution in CFGR
	 * 4. Set the sampling time for channels in SMPR
	 * 5. Set the regular channel sequence and length in SQR
	 * 6. Set the analog mode for GPIO PF7
	 * 7. Disable Deep-power-down
	 * 8. Enable voltage regulator
	 * ***********************************/

	/*1. Enable ADC and GPIO clock*/
	RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOCEN | RCC_AHB4ENR_ADC3EN);			//Enable GPIOC & ADC3 clock

	/*2. Set the prescaler and clock mode*/
	ADC3_COMMON->CCR &= ~ADC_CCR_PRESC_0;								//Input ADC clock not divided
	ADC3_COMMON->CCR |= ADC_CCR_CKMODE_0;								//adc_sclk/1 (Synchronous clock mode)

	/*3. Set the resolution*/
	ADC3->CFGR &= ADC_CFGR_RES_0;										//16 bit resolution

	/*4. Set the sampling time for channels*/
	ADC3->SMPR1 &= ~ADC_SMPR1_SMP1_0;									//1.5 ADC clock cycles

	/*5. Set the regular channel sequence and length*/
	ADC3->SQR1 |= ADC_SQR1_SQ1_0;										//1st CH1
	ADC3->SQR1 &= ~ADC_SQR1_L_0;										//Sequence length 1 conversion

	/*6. Set the analog mode for GPIO PC3*/
	GPIOC->MODER |= (GPIO_MODER_MODE3_0 | GPIO_MODER_MODE3_1);		//Analog mode

	/*7. Disable Deep-power-down*/
	ADC3->CR &= ~ADC_CR_DEEPPWD;										//Disable deep sleep

	/*8. Enable voltage regulator*/
	ADC3->CR |= ADC_CR_ADVREGEN;										//Enable voltage regulator
}

void ADC_cal(void)
{
	/********** STEPS TO FOLLOW ***********
	* 1. Disable Deep Power Down, Enable Voltage regulator
	* 2. Disable ADC
	* 3. Select the input mode for calibration
	* 4. Start calibration
	* 5. Wait until calibration is finished
	* 6. Enable ADC and stop it
	* 7. Save calibration results
	* 8. Disable ADC
	* ***********************************/

	/*1. Disable Deep Power Down, Enable Voltage regulator*/
	ADC3->CR &= ~ADC_CR_DEEPPWD;										//Disable deep sleep
	ADC3->CR |= ADC_CR_ADVREGEN;										//Enable voltage regulator
	while (!(ADC3->ISR & ADC_ISR_LDORDY));								//Wait for LDO RDY flag to be set

	/*2. Disable ADC*/
	ADC3->CR &= ~ADC_CR_ADEN;											//Disable ADC

	/*3. Select the input mode for calibration*/
	ADC3->CR &= ~ADC_CR_ADCALDIF;										//Calibration in Single-ended inputs Mode
	//ADC3->CR |= ADC_CR_ADCALLIN;										//Calibration with the Linearity calibration

	/*4. Start calibration*/
	ADC3->CR |= ADC_CR_ADCAL;											//Starting calibration

	/*5. Wait until calibration is finished*/
	while (!(ADC3->CR & ADC_CR_ADCAL));									//Wait until finished

	/*6. Enable ADC and stop it*/
	ADC3->CR |= ADC_CR_ADEN;											//Enable ADC
	ADC3->CR &= ADC_CR_ADSTART;											//Ensure ADC is stopped

	/*7. Save calibration results*/
	adc_cal_off = (ADC3->CALFACT & 0x07FF);								//Saving calibration offset result
	//adc_cal_lin = (ADC3->CALFACT2 & 0x3FFFFFFF);						//Saving calibration linearity result

	/*8. Disable ADC*/
	ADC3->CR &= ~ADC_CR_ADEN;											//Disable ADC
}

void ADC_en(void)
{
	/********** STEPS TO FOLLOW ***********
	* 1. Disable sleep, Enable LDO, wait to stabilize
	* 2. Enable ADC
	* 3. Wait for ADC stabilization
	* ***********************************/

	/*1. Disable sleep, Enable LDO, wait to stabilize*/
	ADC3->CR &= ~ADC_CR_DEEPPWD;										//Disable deep sleep
	ADC3->CR |= ADC_CR_ADVREGEN;										//Enable voltage regulator
	while (!(ADC3->ISR & ADC_ISR_LDORDY));								//Wait for LDO RDY flag to be set

	/*2. Enable ADC*/
	ADC3->CR |= ADC_CR_ADEN;											//Enable ADC

	/*3. Wait for ADC stabilization*/
	uint32_t delay = 10000;
	while (delay--);													//Wait ~10uS
}

void ADC_dis(void)
{
	/*Disable ADC*/
	ADC3->CR &= ~ADC_CR_ADEN;											//Disable ADC
}

void ADC_start(void)
{
	/*Start conversion*/
	ADC3->CR |= ADC_CR_ADSTART;											//Start conversion
}

uint32_t ADC_read(void)
{
	/*Wait for conversion to be completed*/
	while(!(ADC3->ISR & ADC_ISR_EOC));									//Wait for current conversion to stop

	/*Read result*/
	return (ADC3->DR);													//Return result
}
