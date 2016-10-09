#pragma once

#include "stm32f4xx.h"

struct CanMsg
{
	uint32_t	index;
	uint32_t 	data[2];
	uint8_t		dataNumber;
};


class CanDrv
{
	static const uint8_t queueSize = 0x10;

public:
	uint8_t freeMailbox;

	CanMsg dataRx[queueSize];
	CanMsg dataTx[queueSize];

	CanMsg * currentTx;

	uint32_t indexRx;
	uint32_t indexTx;

	void Init()
	{



	}

	void InitHardware()
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
		while (!(CAN1->MSR & CAN_MSR_INAK)) {};

		//CAN1->MCR |= CAN_MCR_NART;
		CAN1->BTR &= ~(CAN_BTR_SJW | CAN_BTR_TS1 | CAN_BTR_TS2 | CAN_BTR_BRP);

		// 125kbps
		CAN1->BTR |= ((13-1) << 16) | ((2-1) << 20) | (21-1);

		//1Mbps
		//CAN1->BTR |= ((11-1) << 16) | ((2-1) << 20) | (3-1);


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

		//acivate filter
		CAN1->FA1R = CAN_FA1R_FACT0;

		//set masks
		CAN1->sFilterRegister[0].FR1 = 0x00000000;
		CAN1->sFilterRegister[0].FR2 = 0x00000000;

		//end filter initialization
		CAN1->FMR &= ~CAN_FMR_FINIT;

		//Request switching off initialization mode
		CAN1->MCR &= ~CAN_MCR_INRQ;
		while (CAN1->MSR & CAN_MSR_INAK) {};

		//NVIC_EnableIRQ(CAN1_TX_IRQn);
		NVIC_EnableIRQ(CAN1_RX0_IRQn);

	}

	void SetWrData()
	{
		SelectFreeMailbox();
		CAN1->sTxMailBox[freeMailbox].TIR = dataTx[indexTx].index << 21;
		CAN1->sTxMailBox[freeMailbox].TDLR = dataTx[indexTx].data[0];
		CAN1->sTxMailBox[freeMailbox].TDHR = dataTx[indexTx].data[1];
		CAN1->sTxMailBox[freeMailbox].TDTR &= ~0xF;
		CAN1->sTxMailBox[freeMailbox].TDTR |= 0x8;

	}


	void SendTrigger()
	{
		CAN1->sTxMailBox[freeMailbox].TIR |= CAN_TI0R_TXRQ;
	}


	bool SelectFreeMailbox()
	{
		uint32_t mask = CAN_TSR_TME0;
		uint32_t status = CAN1->TSR;

		// sprawdz, czy znaleziono mailbox!
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

/*
	CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer)
	{
	    CO_ReturnError_t err = CO_ERROR_NO;
	    int8_t txBuff;

	    if(buffer->bufferFull)
	    {
	        if(!CANmodule->firstCANtxMessage)
	            CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_CAN_TX_OVERFLOW, CO_EMC_CAN_OVERRUN, 0);
	        err = CO_ERROR_TX_OVERFLOW;
	    }

	    CO_DISABLE_INTERRUPTS();
	    //if CAN TB buffer0 is free, copy message to it
	     txBuff = getFreeTxBuff(CANmodule);

	     CO_CANsendToModule(CANmodule, buffer, txBuff);
	    }
	    //if no buffer is free, message will be sent by interrupt
	    else
	    {
	        buffer->bufferFull = 1;
	        CANmodule->CANtxCount++;
	        // vsechny buffery jsou plny, musime povolit preruseni od vysilace, odvysilat az v preruseni
	        CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, ENABLE);
	    }
	    CO_ENABLE_INTERRUPTS();

	    return err;
	}*/

/*
	void CO_CANverifyErrors(CO_CANmodule_t *CANmodule)
	{
	   uint32_t err;
	   CO_EM_t* em = (CO_EM_t*)CANmodule->em;

	   err = CANmodule->CANbaseAddress->ESR;
	   // if(CAN_REG(CANmodule->CANbaseAddress, C_INTF) & 4) err |= 0x80;

	   if(CANmodule->errOld != err)
	   {
	      CANmodule->errOld = err;

	      //CAN RX bus overflow
	      if(CANmodule->CANbaseAddress->RF0R & 0x08)
	      {
	         CO_errorReport(em, CO_EM_CAN_RXB_OVERFLOW, CO_EMC_CAN_OVERRUN, err);
	         CANmodule->CANbaseAddress->RF0R &=~0x08;//clear bits
	      }

	      //CAN TX bus off
	      if(err & 0x04) CO_errorReport(em, CO_EM_CAN_TX_BUS_OFF, CO_EMC_BUS_OFF_RECOVERED, err);
	      else           CO_errorReset(em, CO_EM_CAN_TX_BUS_OFF, err);

	      //CAN TX or RX bus passive
	      if(err & 0x02)
	      {
	         if(!CANmodule->firstCANtxMessage) CO_errorReport(em, CO_EM_CAN_TX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
	      }
	      else
	      {
	        // int16_t wasCleared;
	        CO_errorReset(em, CO_EM_CAN_TX_BUS_PASSIVE, err);
	        CO_errorReset(em, CO_EM_CAN_TX_OVERFLOW, err);
	      }


	      if(err & 0x01)
	      {
	         CO_errorReport(em, CO_EM_CAN_BUS_WARNING, CO_EMC_NO_ERROR, err);
	      }
	      else
	      {
	         CO_errorReset(em, CO_EM_CAN_BUS_WARNING, err);
	      }
	   }
	}*/

	/*
	void CO_CANinterrupt_Rx(CO_CANmodule_t *CANmodule)
	{
		CanRxMsg      CAN1_RxMsg;

		CAN_Receive(CANmodule->CANbaseAddress, CAN_FilterFIFO0, &CAN1_RxMsg);
		{
		        uint16_t index;
		        uint8_t msgMatched = 0;
		        CO_CANrx_t *msgBuff = CANmodule->rxArray;
		        for (index = 0; index < CANmodule->rxSize; index++)
		        {
		            uint16_t msg = (CAN1_RxMsg.StdId << 2) | (CAN1_RxMsg.RTR ? 2 : 0);
		            if (((msg ^ msgBuff->ident) & msgBuff->mask) == 0)
		            {
		                msgMatched = 1;
		                break;
		            }
		            msgBuff++;
		        }
		        //Call specific function, which will process the message
		        if (msgMatched && msgBuff->pFunct)
	            msgBuff->pFunct(msgBuff->object, &CAN1_RxMsg);
			}
	}*/

	/******************************************************************************/
	// Interrupt from Transeiver
	/*void CO_CANinterrupt_Tx(CO_CANmodule_t *CANmodule)
	{

	     int8_t txBuff;

	    CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, DISABLE); // Transmit mailbox empty interrupt

	    CANmodule->firstCANtxMessage = 0;

	    CANmodule->bufferInhibitFlag = 0;

	    if(CANmodule->CANtxCount > 0)
	    {
	        uint16_t i;


	        CO_CANtx_t *buffer = CANmodule->txArray;

	        for(i = CANmodule->txSize; i > 0; i--)
	        {

	            if(buffer->bufferFull)
	            {
	                buffer->bufferFull = 0;
	                CANmodule->CANtxCount--;
	    txBuff = getFreeTxBuff(CANmodule);    //VJ

	                CANmodule->bufferInhibitFlag = buffer->syncFlag;
	                CO_CANsendToModule(CANmodule, buffer, txBuff);
	                break;
	            }
	            buffer++;
	        }


	        if(i == 0) CANmodule->CANtxCount = 0;
	    }
	}*/

	/******************************************************************************/
	/*void CO_CANinterrupt_Status(CO_CANmodule_t *CANmodule)
	{
	  // status is evalved with pooling
	}*/

	void Receive()
	{
		bool isNewData;
		do
		{
			// check pending messages
			if (CAN1->RF0R & CAN_RF0R_FMP0)
			{
				uint32_t data;

				data = CAN1->sFIFOMailBox[0].RIR;
				dataRx[indexRx].index = data >> 21;

				data = CAN1->sFIFOMailBox[0].RDTR;
				dataRx[indexRx].dataNumber = (uint8_t) (data & CAN_RDT0R_DLC);

				data = CAN1->sFIFOMailBox[0].RDLR;
				dataRx[indexRx].data[0] = data;

				data = CAN1->sFIFOMailBox[0].RDHR;
				dataRx[indexRx].data[1] = data;

				CAN1->RF0R |= CAN_RF0R_RFOM0;

				indexRx++;
				indexRx %= queueSize;
				isNewData = true;
			}
			else isNewData = false;
		}
		while(isNewData);
	}
	/******************************************************************************/
	void SendStart()
	{


/*	    CAN_Transmit(CANmodule->CANbaseAddress, &CAN1_TxMsg);
	    CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, ENABLE);*/

	    /*Test code, VJ using Drivers End*/

	}

	void IrqWrite()
	{

	}

	void IrqRead()
	{
		Receive();
	}
};

