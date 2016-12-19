#include "stm32f4xx_conf.h"


#include "sdo.h"
#include "pdo.h"
#include "can_drv.h"
#include "mi_controller.h"
#include "led_interface.h"

#define CPU_CLK 168000000L

bool tick = false;
CanDrv canDrv;
Sdo* SDO;



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


void SysTick_Handler(void)
{
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
	SDO->received = true;
	SDO->StackUpdate();
	/*
	if (!CommandQueue.empty())
	{
		sdo.PushCommand(CommandQueue.Next());
		sdo.PrepareData();

		// test -> przekazanie rozkazu do sterownika Can
		canDrv.dataTx[0].index = sdo.idWr;
		canDrv.dataTx[0].data[0] = sdo.mailboxData[0];
		canDrv.dataTx[0].data[1] = sdo.mailboxData[1];
		canDrv.dataTx[0].dataNumber = 8;

		canDrv.SetWrData();
	}*/
}



int main(void)
{
	Sdo sdo(&canDrv, 1);
	Pdo pdo(&canDrv, 1);
	SDO = &sdo;
	MiControlCmds Command;

	SystemInit();

	if (SysTick_Config(CPU_CLK/1000))
	{ // ustawienie zegara systemowego w programie
		while (1);
	}

	Led::Init();
	canDrv.Init(CanDrv::B1M);

	__enable_irq();


	sdo.PushCommand(Command.ClearError());
	sdo.PushCommand(Command.RestoreParam()); // Wyd³u¿yæ czas wykonania?
	sdo.PushCommand(Command.MotorEnable());

	sdo.PushCommand(Command.DisableRPDO());
	sdo.PushCommand(Command.MapRPDO(1, 0x3500, 0, 32));
	sdo.PushCommand(Command.EnableRPDO(1));

	sdo.StartSequence();

    while(!pdo.Operational)
    	if (tick)
    	{
    		tick = false;
    		if (!sdo.completed) sdo.SendTrigger();
    		else pdo.SetOperational();
    	}

    while (true) if (tick) { pdo.Send(100); break; }

    return 0;
}
