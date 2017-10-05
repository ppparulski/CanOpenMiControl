#pragma once

#include "can_drv.hpp"
#include "stdint.h"
#include "stddef.h"
#include "string.h"

class Nmt
{
	CanDrv * canDrv;
	uint16_t node;

public:
	Nmt(CanDrv * canDrv, uint16_t id) : canDrv(canDrv), node(id) {}

	void GoToOperational()
	{
		CanMsg canMsg;

		canMsg.index = 0;
		canMsg.data[0] = 0x0100 | node;
		canMsg.data[1] = 0;
		canMsg.dataNumber = 2;

		canDrv->SetTxMsg(canMsg);
	}

	void Stop()
	{
		CanMsg canMsg;

		canMsg.index = 0;
		canMsg.data[0] = 0x0000 | node;
		canMsg.data[1] = 0;
		canMsg.dataNumber = 2;

		canDrv->SetTxMsg(canMsg);

	}


	void GoToPreoperational()
	{
		CanMsg canMsg;

		canMsg.index = 0;
		canMsg.data[0] = 0x8000 | node;
		canMsg.data[1] = 0;
		canMsg.dataNumber = 2;

		canDrv->SetTxMsg(canMsg);
	}

	void ResetPreoperational()
	{
		CanMsg canMsg;

		canMsg.index = 0;
		canMsg.data[0] = 0x8000 | node;
		canMsg.data[1] = 0;
		canMsg.dataNumber = 2;

		canDrv->SetTxMsg(canMsg);
	}
};
