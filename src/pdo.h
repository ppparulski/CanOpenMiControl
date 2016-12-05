#pragma once

#include "can_drv.h"
#include "stdint.h"
#include "stddef.h"
#include "string.h"


class Pdo
{
	static const uint8_t stackSize = 32;
	static const uint16_t idWrOffset = 0x600;
	static const uint16_t idRdOffset = 0x580;

	CanDrv * canDrv;

	uint32_t mailboxData[2];
	uint8_t id;
	uint16_t idWr;
	uint16_t idRd;

public:
	Pdo(CanDrv * canDrv, uint16_t id)
	{
		this->id = id;
		this->canDrv = canDrv;
		idWr = id + idWrOffset;
		idRd = id + idRdOffset;
	}

	void Send(int Value)
	{
		canDrv->dataTx[0].index = idWr;
		canDrv->dataTx[0].data[0] = Value;
		canDrv->dataTx[0].data[1] = Value >> 8;
		canDrv->dataTx[0].dataNumber = 8;

		canDrv->SetWrData();

		canDrv->SendTrigger();
	}
};
