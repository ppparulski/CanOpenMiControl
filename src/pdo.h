#pragma once

#include "can_drv.h"
#include "stdint.h"
#include "stddef.h"
#include "string.h"


class Pdo
{
	bool operational;
	CanDrv * canDrv;

public:
	const bool& Operational;
	Pdo(CanDrv * canDrv, uint16_t id) : canDrv(canDrv), operational(false), Operational(operational) {}

	void SetOperational()
	{
		canDrv->dataTx[0].index = 0;
		canDrv->dataTx[0].data[0] = 0x0101;
		canDrv->dataTx[0].data[1] = 0;
		canDrv->dataTx[0].dataNumber = 2;

		canDrv->SetWrData();
		canDrv->SendTrigger();

		operational = true;
	}

	void Send(int Value)
	{
		canDrv->dataTx[0].index = 0x201;
		canDrv->dataTx[0].data[0] = Value;
		canDrv->dataTx[0].data[1] = Value >> 8;
		canDrv->dataTx[0].dataNumber = 2;

		canDrv->SetWrData();
		canDrv->SendTrigger();
	}

	void Read(int Index)
	{
		canDrv->dataTx[0].index = Index;
		canDrv->dataTx[0].data[0] = 0;
		canDrv->dataTx[0].data[1] = 0;
		canDrv->dataTx[0].dataNumber = 0;

		canDrv->SetWrData();
		canDrv->SendTrigger();
	}
};
