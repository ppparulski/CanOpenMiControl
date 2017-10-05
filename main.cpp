#include "stm32f4xx_conf.h"


#include "sdo.hpp"
#include "pdo.hpp"
#include "can_drv.hpp"
#include "mi_control.hpp"
#include "led_interface.h"
#include "motor_drv.hpp"

#include "can_open.hpp"

#include "math.h"


#define CPU_CLK 168000000L

volatile bool tick;

CanDrv canDrv;
MotorDrv * motor;



extern "C"
{
	void SysTick_Handler(void);
	void CAN1_RX0_IRQHandler(void);
	void CAN1_TX_IRQHandler(void);
}



volatile uint32_t counter1 = 0, cnt2 = 0;
void GeneralHardwareInit()
{
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
	if (++counter1==2)
	{
		counter1 = 0;
		Led::Yellow() ^= 1;
		tick = true;
	}
}

int Angle[1000], Current[1000], Velocity[1000], Desired[1000];
int Index = 0, Index2 = 0, Index3;

void CAN1_TX_IRQHandler(void)
{
	canDrv.IrqWrite();
}

void CAN1_RX0_IRQHandler(void)
{
	canDrv.IrqRead();

	while (canDrv.GetRxMsg())
	{
		auto id = canDrv.rxMsg->index & CanOpenObjects::DeviceMask;
		auto cob = canDrv.rxMsg->index & CanOpenObjects::ObjectMask;

		switch(cob)
		{
			case CanOpenObjects::sdoRx:
				motor->sdo.received = true;
			break;

			case CanOpenObjects::pdo1Rx:

		    break;

			case CanOpenObjects::pdo2Rx:
		    break;

			case CanOpenObjects::pdo3Rx:
		    break;

			case CanOpenObjects::pdo4Rx:
		    break;

			default:
			break;

		}
	}


}



int main(void)
{
	tick = false;
	SystemInit();

	if (SysTick_Config(CPU_CLK/1000))
	{
		while (1);
	}

	Led::Init();
	canDrv.Init(CanDrv::B1M);

	MotorDrv motorDrv(&canDrv, 1);
	motor = &motorDrv;


	motor->Configure();
	canDrv.SendStart();


	//canDrv.SendTrigger();

	__enable_irq();


	while(true)
	{
    	if (tick)
    	{
    		tick = false;

    		switch (motor->state)
    		{
    			case MotorDrv::Idle:
    				if (motor->sdo.StackWriteUpdate())
    					canDrv.SendStart();
    				if (motor->sdo.completed)
    					motor->state = MotorDrv::Configured;
    		    break;

    			case MotorDrv::Configured:
    				motor->nmt.GoToOperational();
    				canDrv.SendStart();
    				motor->state = MotorDrv::Operational;
    			break;

    			case MotorDrv::Operational:
    				motor->SetVelocity(20);
    				SendSynchObj(&canDrv);
    				canDrv.SendStart();
    			break;
    		}
    	}
	}
    return 0;
}
