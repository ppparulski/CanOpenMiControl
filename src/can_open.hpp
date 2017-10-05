#pragma once

#include "can_drv.hpp"

struct CanOpenObjects
{
	const static uint16_t ObjectMask = 0x780;
	const static uint16_t DeviceMask = 0x07F;

	const static uint16_t pdo1Rx = 0x180;
	const static uint16_t pdo1Tx = 0x200;
	const static uint16_t pdo2Rx = 0x280;
	const static uint16_t pdo2Tx = 0x300;
	const static uint16_t pdo3Rx = 0x380;
	const static uint16_t pdo3Tx = 0x400;
	const static uint16_t pdo4Rx = 0x480;
	const static uint16_t pdo4Tx = 0x500;

	const static uint16_t sdoRx = 0x580;
	const static uint16_t sdoTx = 0x600;



};

inline void SendSynchObj(CanDrv * drv)
{
	CanMsg canMsg;

	canMsg.index = 0x80;
	canMsg.data[0] = 0;
	canMsg.data[1] = 0;
	canMsg.dataNumber = 0;

	drv->SetTxMsg(canMsg);
}

