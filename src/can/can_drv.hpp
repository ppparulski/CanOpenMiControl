#pragma once

#include "stm32f4xx.h"
#include <stddef.h>
#include "mem.hpp"
#include <string.h>

struct CanMsg
{
	uint32_t	index;
	uint32_t 	data[2];
	uint8_t		dataNumber;
};


class CanDrv
{
	static const uint8_t queueSize = 32;
	static const uint8_t queueSizeMask = queueSize-1;



public:

	enum BaudRate
	{
		B125K,
		B1M
	};

	uint8_t freeMailbox;

	bool isTxData;

	volatile CanMsg dataRx[queueSize];
	volatile CanMsg dataTx[queueSize];

	volatile CanMsg * txMsg;
	volatile CanMsg * rxMsg;

	volatile uint8_t indexRxStore;
	volatile uint8_t indexRxLoad;

	volatile uint8_t indexTxStore;
	volatile uint8_t indexTxLoad;

	uint16_t recCounter;


	void Init(const BaudRate br)
	{

		indexRxStore = indexRxLoad = 0;
		indexTxStore = indexTxLoad = 0;

		InitHardware(br);
	}

	void InitHardware(const BaudRate br)
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
		RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;

	    // PD0, PD1 -> CAN: Rx, Tx
		GPIOD->AFR[0] |= (uint32_t) 9 << 0*4 | (uint32_t) 9 << 1*4;
		GPIOD->MODER |=  GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1;
		GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1;
		GPIOD->ODR |= GPIO_ODR_ODR_1;
		GPIOD->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0;

		// switch to initialization mode
		CAN1->MCR &= ~CAN_MCR_SLEEP;
		CAN1->MCR |= CAN_MCR_INRQ;
		CAN1->MCR |= CAN_MCR_ABOM;
		while (!(CAN1->MSR & CAN_MSR_INAK)) {};

		//CAN1->MCR |= CAN_MCR_NART;
		CAN1->BTR &= ~(CAN_BTR_SJW | CAN_BTR_TS1 | CAN_BTR_TS2 | CAN_BTR_BRP);

		// set baudrate
		switch (br)
		{
			case B1M:
				CAN1->BTR |= ((11-1) << 16) | ((2-1) << 20) | (3-1);
			break;

			case B125K:
			default:
				CAN1->BTR |= ((13-1) << 16) | ((2-1) << 20) | (21-1);
			break;

		}

		CAN1->IER |= CAN_IER_FMPIE0;

		//begin filter initialization
		CAN1->FMR |= CAN_FMR_FINIT;

		//reset FA1R
		CAN1->FA1R = 0;

		//single 32 bit scale configuration
		CAN1->FS1R = CAN_FS1R_FSC0;

		//mask mode
		CAN1->FM1R &= ~CAN_FM1R_FBM0;

		//assigned to FIFO1
		CAN1->FFA1R &= ~CAN_FFA1R_FFA0;

		//activate filter
		CAN1->FA1R = CAN_FA1R_FACT0;

		//set masks
		CAN1->sFilterRegister[0].FR1 = 0x00000000;
		CAN1->sFilterRegister[0].FR2 = 0x00000000;

		//end filter initialization
		CAN1->FMR &= ~CAN_FMR_FINIT;

		//Request switching off initialization mode
		CAN1->MCR &= ~CAN_MCR_INRQ;
		while (CAN1->MSR & CAN_MSR_INAK) {};

		NVIC_EnableIRQ(CAN1_TX_IRQn);
		NVIC_EnableIRQ(CAN1_RX0_IRQn);

	}

	void SendTrigger()
	{
		CAN1->sTxMailBox[freeMailbox].TIR |= CAN_TI0R_TXRQ;
	}

	bool SelectFreeMailbox()
	{
		uint32_t mask = CAN_TSR_TME0;
		volatile uint32_t status = CAN1->TSR;

		// check if free mailbox is found!
		for(size_t mailboxNumber = 0; mailboxNumber < 2; mailboxNumber++)
		{
			if (status & mask)
			{
				freeMailbox = mailboxNumber;
				return true;
			}
			mask <<= 1;
		}
		return false;
	}

	void Receive()
	{
		while (CAN1->RF0R & CAN_RF0R_FMP0)
		{
			uint32_t data;

			data = CAN1->sFIFOMailBox[0].RIR;
			dataRx[indexRxStore].index = data >> 21;

			data = CAN1->sFIFOMailBox[0].RDTR;
			dataRx[indexRxStore].dataNumber = (uint8_t) (data & CAN_RDT0R_DLC);

			data = CAN1->sFIFOMailBox[0].RDLR;
			dataRx[indexRxStore].data[0] = data;

			data = CAN1->sFIFOMailBox[0].RDHR;
			dataRx[indexRxStore].data[1] = data;

			CAN1->RF0R |= CAN_RF0R_RFOM0;

			indexRxStore++;
			indexRxStore &= queueSizeMask;
		}
	}
	/******************************************************************************/


	bool GetRxMsg()
	{
		if ((indexRxStore-indexRxLoad) & queueSizeMask)
		{
			rxMsg = &dataRx[indexRxLoad++];
			indexRxLoad &= queueSizeMask;
			return true;
		}
		else
			return false;
	}

	void SetTxMsg(CanMsg &m)
	{
		//DataCopy((uint8_t*)&dataTx[indexTxStore++], (uint8_t*)&m, sizeof(CanMsg));
		memcpy((void*)&dataTx[indexTxStore++], (void*)&m, sizeof(CanMsg));
		indexTxStore &= queueSizeMask;
	}

	bool GetTxMsg()
	{
		if ((indexTxStore-indexTxLoad) & queueSizeMask)
		{
			txMsg = &dataTx[indexTxLoad];
			return true;
		}
		else
			return false;
	}

	void SendStart()
	{
		SendTrigger();
		CAN1->IER |= CAN_IER_TMEIE;
	}

	void IrqWrite()
	{
		if (GetTxMsg())
		{
			if (SelectFreeMailbox())
			{
				CAN1->sTxMailBox[freeMailbox].TIR = txMsg->index << 21;
				CAN1->sTxMailBox[freeMailbox].TDLR = txMsg->data[0];
				CAN1->sTxMailBox[freeMailbox].TDHR = txMsg->data[1];
				CAN1->sTxMailBox[freeMailbox].TDTR &= ~0xF;
				CAN1->sTxMailBox[freeMailbox].TDTR |= txMsg->dataNumber;
				indexTxLoad++;
				indexTxLoad &= queueSizeMask;
				SendTrigger();
			}
		}
		else
		{
			CAN1->IER &= ~CAN_IER_TMEIE;
		}
	}

	void IrqRead()
	{
		Receive();
	}
};

