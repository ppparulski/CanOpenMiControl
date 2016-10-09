#pragma once

#include "stm32f4xx.h"
#include "hdr_bitband.h"

// definicja zmiennych bitowych dla ukladow peryferyjnych
//#define LED_G bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 12)
//#define LED_Y bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 13)
//#define LED_R bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 14)
//#define LED_B bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 15)

class Led
{
	static void HardwareInit()
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

		// PD12, PD13, PD14, PD15 - LEDy
		GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 |GPIO_MODER_MODER15_0;
	}

public:

	static void Init()
	{
		HardwareInit();
	}

	static volatile unsigned long & Green()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 12);
	}

	static volatile unsigned long & Yellow()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 13);
	}

	static volatile unsigned long & Red()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 14);
	}

	static volatile unsigned long & Blue()
	{
		return bitband_t m_BITBAND_PERIPH(&GPIOD->ODR, 15);
	}

};
