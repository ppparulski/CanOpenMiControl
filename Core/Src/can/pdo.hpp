#pragma once

#include "can_drv.hpp"
#include "stdint.h"
#include "stddef.h"
#include "can_open.hpp"


typedef struct
{
	uint16_t index;
	uint32_t data0;
	uint32_t data1;
	uint8_t size;
} __attribute__ ((packed)) PdoCmd;


class Pdo
{
	bool operational;
	CanDrv * canDrv;

public:

	enum PdoObj {Pdo1 = 0, Pdo2 = 1, Pdo3 = 2, Pdo4 = 3};
	PdoCmd cmdTx[4];
	PdoCmd cmdRx[4];

	Pdo(CanDrv * canDrv, uint16_t id) : canDrv(canDrv)
	{
		cmdRx[Pdo1].index = id + CanOpenObjects::pdo1Rx;
		cmdRx[Pdo2].index = id + CanOpenObjects::pdo2Rx;
		cmdRx[Pdo3].index = id + CanOpenObjects::pdo3Rx;
		cmdRx[Pdo4].index = id + CanOpenObjects::pdo4Rx;

		cmdTx[Pdo1].index = id + CanOpenObjects::pdo1Tx;
		cmdTx[Pdo2].index = id + CanOpenObjects::pdo2Tx;
		cmdTx[Pdo3].index = id + CanOpenObjects::pdo3Tx;
		cmdTx[Pdo4].index = id + CanOpenObjects::pdo4Tx;
	};

	void Send(PdoObj pdoObj)
	{
		CanMsg canMsg;

		canMsg.index = cmdTx[pdoObj].index;
		canMsg.data[0] = cmdTx[pdoObj].data0;
		canMsg.data[1] = cmdTx[pdoObj].data1;
		canMsg.dataNumber = cmdTx[pdoObj].size;

		canDrv->SetTxMsg(canMsg);
	}

/*	void Read(int Index)
	{
		canDrv->dataTx.index = Index;
		canDrv->dataTx.data[0] = 0;
		canDrv->dataTx.data[1] = 0;
		canDrv->dataTx.dataNumber = 0;

		canDrv->SetWrData();
		canDrv->SendTrigger();
	}*/
};
