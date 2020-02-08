#include "stm32f4xx_conf.h"


#include "sdo.hpp"
#include "pdo.hpp"
#include "can_drv.hpp"
#include "mi_control.hpp"
#include "motor_drv.hpp"
#include "led_interface.h"


#include "can_open.hpp"

#include "math.h"


#define CPU_CLK 168000000L

volatile bool tick;

CanDrv canDrv;
MotorDrv * motor1;
MotorDrv * motor2;

uint16_t ID_MOTOR_1;
uint16_t ID_MOTOR_2;


uint32_t appClk;

extern "C"
{
	void SysTick_Handler(void);
	void CAN1_RX0_IRQHandler(void);
	void CAN1_TX_IRQHandler(void);
}



volatile uint32_t counter1 = 0, cnt2 = 0;

volatile uint32_t indexTable_1 = 0;
volatile uint32_t indexTable_2 = 0;
volatile int32_t bufData_1[1024];
volatile int32_t bufData_2[1024];


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
	appClk++;
	if (++counter1==2)
	{
		counter1 = 0;
		Led::Yellow() ^= 1;
		tick = true;
	}
}

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
				if(id==ID_MOTOR_1)
					motor1->sdo.received = true;
				else if (id==ID_MOTOR_2)
					motor2->sdo.received = true;
			break;

			case CanOpenObjects::pdo1Rx:
				if(id==ID_MOTOR_1){

					motor1->ReadPosition(canDrv.rxMsg);	//read motor1 position
					motor1->ReadVelocity(canDrv.rxMsg);	//read motor1 velocity

					//Store data
					bufData_1[indexTable_1++] = motor1->measuredPos;
					indexTable_1 &= 1024-1;
					bufData_2[indexTable_2++] = motor1->measuredVel;
					indexTable_2 &= 1024-1;

				} else if (id==ID_MOTOR_2){
					motor2->ReadPosition(canDrv.rxMsg);	//read motor2 position
					motor2->ReadVelocity(canDrv.rxMsg);	//read motor2 velocity
				}
		    break;

			case CanOpenObjects::pdo2Rx:
				if(id==ID_MOTOR_1){
					motor1->ReadCurrent(canDrv.rxMsg);	//read motor1 current
					motor1->ReadStatus(canDrv.rxMsg);	//read motor1 status
				} else if(id==ID_MOTOR_2){
					motor2->ReadCurrent(canDrv.rxMsg);	//read motor2 current
					motor2->ReadStatus(canDrv.rxMsg);	//read motor2 status
				}
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

	ID_MOTOR_1 = 3;
	ID_MOTOR_2 = 1;

	appClk = 0;
	SystemInit();

	if (SysTick_Config(CPU_CLK/1000))
	{
		while (1);
	}

	Led::Init();
	canDrv.Init(CanDrv::B1M);	//set CAN baudrate to 1 MBit


	MotorDrv motorDrv1(&canDrv, ID_MOTOR_1);	//assign first motor id
	motor1 = &motorDrv1;
	MotorDrv motorDrv2(&canDrv, ID_MOTOR_2);	//assign second motor id
	motor2 = &motorDrv2;



	motor1->Configure();
	motor2->Configure();

	canDrv.SendStart();

	__enable_irq();


	while(true)
	{
    	if (tick)
    	{
    		float time = (float) appClk * 0.001;
    		tick = false;


    		///***   Motor 1   ***///

    		switch (motor1->state)
    		{
    			case MotorDrv::Idle:
    				if (motor1->sdo.StackWriteUpdate())
    					canDrv.SendStart();
    				if (motor1->sdo.completed)
    					motor1->state = MotorDrv::Configured;
    		    break;

    			case MotorDrv::Configured:
    				motor1->nmt.GoToOperational();
    				canDrv.SendStart();
    				motor1->state = MotorDrv::Waiting;
    			break;

    			case MotorDrv::Waiting:
    				if((motor1->state == MotorDrv::Waiting || motor1->state == MotorDrv::Operational)
    						&& (motor2->state == MotorDrv::Waiting || motor2->state == MotorDrv::Operational))
    					motor1->state = MotorDrv::Operational;
    			break;

    			case MotorDrv::Operational:
    					motor1->desiredVel = 100.0f*sinf(5*time);
	    				motor1->SetVelocity();
	    				SendSynchObj(&canDrv);
	    				canDrv.SendStart();
    				    			break;
    		}


    		///***   Motor 2   ***///

    		switch (motor2->state)
			{
				case MotorDrv::Idle:
					if (motor2->sdo.StackWriteUpdate())
						canDrv.SendStart();
					if (motor2->sdo.completed)
						motor2->state = MotorDrv::Configured;
				break;

				case MotorDrv::Configured:
					motor2->nmt.GoToOperational();
					canDrv.SendStart();
					motor2->state = MotorDrv::Waiting;
				break;

				case MotorDrv::Waiting:
    				if((motor1->state == MotorDrv::Waiting || motor1->state == MotorDrv::Operational)
    						&& (motor2->state == MotorDrv::Waiting || motor2->state == MotorDrv::Operational))
    					motor2->state = MotorDrv::Operational;
				break;

				case MotorDrv::Operational:
						motor2->desiredVel = 100.0f*sinf(5*time);
						motor2->SetVelocity();
						SendSynchObj(&canDrv);
						canDrv.SendStart();
				break;
			}
    	}
	}
    return 0;
}
