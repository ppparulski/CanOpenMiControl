/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can\sdo.hpp"
#include "can\pdo.hpp"
#include "can\can_drv.hpp"
#include "motor\mi_control.hpp"
#include "motor\motor_drv.hpp"
#include "cpu\led_interface.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern "C"
{
	void SysTick_Handler(void);
	void CAN1_RX0_IRQHandler(void);
	void CAN1_TX_IRQHandler(void);
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile bool tick;

CanDrv canDrv;
MotorDrv * motor1;
MotorDrv * motor2;

uint16_t ID_MOTOR_1;
uint16_t ID_MOTOR_2;


uint32_t appClk;

volatile uint32_t counter1 = 0, cnt2 = 0;

volatile uint32_t indexTable_1 = 0;
volatile uint32_t indexTable_2 = 0;
volatile int32_t bufData_1[1024];
volatile int32_t bufData_2[1024];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  tick = false;

  ID_MOTOR_1 = 3;
  ID_MOTOR_2 = 1;

  appClk = 0;

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
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
	    					motor1->desiredVel = 100.0f; //*sinf(5*time);
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
							motor2->desiredVel = 100.0f; //*sinf(5*time);
							motor2->SetVelocity();
							SendSynchObj(&canDrv);
							canDrv.SendStart();
					break;
				}
	    	}
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void CAN1_TX_IRQHandler(void)
{
	canDrv.IrqWrite();
}

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
	tick = true;
	if (++counter1==500)
	{
		counter1 = 0;
		Led::Yellow() ^= 1;
		//tick = true;
	}
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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
