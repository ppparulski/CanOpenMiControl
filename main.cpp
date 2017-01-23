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



volatile uint32_t counter1 = 0, cnt2 = 0;
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
	cnt2++;
}


unsigned int Angle[1000], Current[1000], Velocity[1000];
int Index = 0, Index2 = 0, Index3;

void CAN1_RX0_IRQHandler(void)
{
	canDrv.IrqRead();
	SDO->StackUpdate();

	// Odczytywanie 2 z 4 bajtów pozycji enkodera.
	if (canDrv.dataRx[canDrv.indexRx].index == 0x181)
		if (Index < 1000) Angle[Index++] = canDrv.dataRx[canDrv.indexRx].data[0];
		else int a = 0;

	// Odczytywanie 2 z 4 bajtów pozycji enkodera.
	if (canDrv.dataRx[canDrv.indexRx].index == 0x281)
		if (Index2 < 1000) Velocity[Index2++] = canDrv.dataRx[canDrv.indexRx].data[0];
		else int a = 0;

	// Odczytywanie 2 z 4 bajtów pozycji enkodera.
	if (canDrv.dataRx[canDrv.indexRx].index == 0x381)
		if (Index3 < 1000) Current[Index3++] = canDrv.dataRx[canDrv.indexRx].data[0];
		else int a = 0;
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
	sdo.PushCommand(Command.RestoreParam());
	sdo.PushCommand(Command.MotorEnable());

	sdo.PushCommand(Command.DisableRPDO());
	sdo.PushCommand(Command.MapRPDO(1, Command.SetSubvel(0), 32)); // Alternatywna sk³adnia: MapRPDO(1, 0x3500, 0, 32)
	sdo.PushCommand(Command.EnableRPDO(1));


	// TPDO 0
	sdo.PushCommand(Command.DisableTPDO(0));
	sdo.PushCommand(Command.TransmissionType(0));
	sdo.PushCommand(Command.MapTPDO(0, 0x3762, 0, 32)); // Pozycja enkodera (liczba impulsów).
	sdo.PushCommand(Command.EnableTPDO(0));

	// TPDO 1
	sdo.PushCommand(Command.DisableTPDO(1));
	sdo.PushCommand(Command.TransmissionType(1));
	sdo.PushCommand(Command.MapTPDO(1, 0x3A04, 1, 32)); // Prêdkosc enkodera.
	sdo.PushCommand(Command.EnableTPDO(1));

	// TPDO 2
	sdo.PushCommand(Command.DisableTPDO(2));
	sdo.PushCommand(Command.TransmissionType(2));
	sdo.PushCommand(Command.MapTPDO(2, 0x3262, 0, 32)); // Pr¹d silnika.
	sdo.PushCommand(Command.EnableTPDO(2));




	sdo.StartSequence();

    while(!pdo.Operational)
    	if (tick)
    	{
    		tick = false;
    		if (!sdo.completed) sdo.SendTrigger();
    		else pdo.SetOperational();
    	}

    while (true) if (tick) { tick = false; pdo.Send(100); break; } // Ustawienie prêdkosci zadanej.
    while (true) if (cnt2 > 20) { cnt2 = 0; pdo.Read(0x80); } // Cykliczne wywo³ywanie pobrania pomiarów.

    return 0;
}
