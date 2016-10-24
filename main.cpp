#include "stm32f4xx_conf.h"


#include "sdo.h"
#include "can_drv.h"
#include "mi_controller.h"
#include "led_interface.h"

#define CPU_CLK 168000000L

bool tick = false;
CanDrv canDrv;




extern "C"
{
	void SysTick_Handler(void);
	void CAN1_RX0_IRQHandler(void);
}



volatile uint32_t counter1 = 0;
void GeneralHardwareInit()
{
	// Inicjalizacja mikrokontrolera



		//**************** port A
		// ustawienie predkosci pracy linii portu
		GPIOA->OSPEEDR = GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR4_1;
		// wybor urzadzen na odpowiednich liniach (wedlug dokumencjacji danego typu procesora)
		// wybor pracy linii portu
		GPIOA->MODER |=  GPIO_MODER_MODER1_0 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 |GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;

		GPIOA->IDR |= GPIO_IDR_IDR_6;

}


void SysTick_Handler(void) {

	if (++counter1==500)
	{
		counter1 = 0;
		Led::Yellow() ^= 1;
		tick = true;
	}

}

void CAN1_RX0_IRQHandler(void)
{
	canDrv.IrqRead();


}


int main(void)
{

	SystemInit();

	if (SysTick_Config(CPU_CLK/1000))
	{ // ustawienie zegara systemowego w programie
		while (1);
	}

	Sdo sdo(&canDrv, 1);
	MiControlCmds miControl;

	Led::Init();
	canDrv.Init(CanDrv::B1M);

	__enable_irq();


	sdo.PushCommand(miControl.ClearError());

	sdo.PrepareData();

	// test -> przekazanie rozkazu do sterownika Can
	canDrv.dataTx[0].index = sdo.idWr;
	canDrv.dataTx[0].data[0] = sdo.mailboxData[0];
	canDrv.dataTx[0].data[1] = sdo.mailboxData[1];
	canDrv.dataTx[0].dataNumber = 8;


	canDrv.SetWrData();


    while(1)
    {
    	if (tick)
    	{
    		tick = false;
    		canDrv.SendTrigger();
    	}

    }
    return 0;
}
