#pragma once

#include "can_drv.hpp"
#include "can_open.hpp"
#include "stdint.h"
#include "stddef.h"
#include "string.h"


typedef struct
{
	uint8_t	type;
	uint16_t index;
	uint8_t subindex;
	uint8_t data[4];
	uint16_t timeout;
	int16_t trials;
} __attribute__ ((packed)) SdoCmd;


class Sdo
{

	static const uint8_t stackSize = 32;
	static const uint16_t idWrOffset = CanOpenObjects::sdoTx;
	static const uint16_t idRdOffset = CanOpenObjects::sdoRx;

	CanDrv * canDrv;

	uint16_t idWr;
	uint16_t idRd;

public:

	SdoCmd cmd[stackSize];								// stos wiadomosci

	volatile uint32_t data;
	volatile uint16_t cmdIndex;
	volatile uint16_t cmdNumber;					// liczba komunikatów do wyslania

	volatile bool	completed;
	volatile bool	started;
	volatile bool	transmitted;
	volatile bool	received;

	volatile bool	startTrigger;

	volatile uint16_t time;
	volatile int16_t	trials;

	uint32_t mailboxData[2];
	uint8_t id;

	Sdo(CanDrv * canDrv, uint16_t id)
	{
		completed = false;
		cmdNumber = 0;
		this->id = id;
		this->canDrv = canDrv;
		cmdIndex = 0;
		idWr = id + idWrOffset;
		idRd = id + idRdOffset;
	}

	void Reset()
	{
		time = cmd[cmdIndex].timeout;
		//mask = m->mbox_tx_mask;
	}

	void PushCommand(const SdoCmd &c)
	{
		//cmd[cmdNumber++] = cmd;
		memcpy((void*)&cmd[cmdNumber], (void*)&c, sizeof(SdoCmd));
		cmdNumber++;
		cmdNumber &= stackSize-1;
	}

	// wype³nienie skrzynki trescia
	void PrepareData()
	{
		SdoCmd * c = &cmd[cmdIndex];

		mailboxData[0] = *(uint32_t*)&c->type;
		mailboxData[1] = *(uint32_t*)c->data;

		//canDrv->dataTx
		time = c->timeout;
		trials = c->trials;
		transmitted = false;
		received = false;

		CanMsg canMsg;

		canMsg.index = idWr;
		canMsg.data[0] = mailboxData[0];
		canMsg.data[1] = mailboxData[1];
		canMsg.dataNumber = 8;

		canDrv->SetTxMsg(canMsg);
	}

	void StartSequence()
	{
		cmdIndex = 0;
		startTrigger = true;
		PrepareData();
		completed = false;
	}

	void StackUpdate()
	{
		if (++cmdIndex < cmdNumber) PrepareData();
		else completed = true;
	}

	bool StackWriteUpdate()
	{
		bool newCmd = false;
		if (completed) return false;
		if (!trials) return false;

		if (!startTrigger)
		{
			startTrigger = false;
			completed = false;
		}
		else
		{
			// czy potwierdzono przeslanie danych?
			if (received)
			{
				cmdIndex++;
				cmdIndex %= stackSize;
				if (cmdIndex < cmdNumber)
				{
					PrepareData();
					newCmd = true;
				}
				else
				{
					completed = true;
				}
			}
			else
			{
				// sprawdzenie warunku czasu
				if (!(--time))
				{
					Reset();
					PrepareData();
					newCmd = true;
					if (trials > 0) trials--;
				}
			}
		}
		return newCmd;
	}

	void CheckAnswer()
	{


	}


};

