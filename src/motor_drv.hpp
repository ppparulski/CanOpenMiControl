#pragma once
#include "mi_control.hpp"
#include "string.h"

class MotorDrv
{

public:
	Sdo sdo;
	Pdo pdo;
	Nmt nmt;

	enum State {Idle = 0, Configured = 1, Operational = 2};

	State state;

	MotorDrv(CanDrv * canDrv, uint8_t id) : sdo(canDrv, id), pdo(canDrv, id), nmt(canDrv, id)
	{
		state = Idle;
		SetPdoCmds();

	}

	void SetPdoCmds()
	{
		pdo.cmdTx[Pdo::Pdo1].size = 4;
	}

	void Configure()
	{
		sdo.PushCommand(MiControlCmds::ClearError());
		sdo.PushCommand(MiControlCmds::RestoreParam());
		sdo.PushCommand(MiControlCmds::MotorEnable());

		sdo.PushCommand(MiControlCmds::DisableRPDO());
		sdo.PushCommand(MiControlCmds::MapRPDO(1, MiControlCmds::SetSubvel(0), MiControlCmds::DataSize32)); // Alternatywna sk³adnia: MapRPDO(1, 0x3500, 0, 32)
		sdo.PushCommand(MiControlCmds::EnableRPDO(1));

		// TPDO 0
		sdo.PushCommand(MiControlCmds::DisableTPDO(0));
		sdo.PushCommand(MiControlCmds::SetTransmissionType(0));
		sdo.PushCommand(MiControlCmds::MapTPDO(0, 0x3762, 0, MiControlCmds::DataSize32)); // Pozycja enkodera (liczba impulsów).
		sdo.PushCommand(MiControlCmds::EnableTPDO(1, 1));

		// TPDO 1
		sdo.PushCommand(MiControlCmds::DisableTPDO(1));
		sdo.PushCommand(MiControlCmds::SetTransmissionType(1));
		sdo.PushCommand(MiControlCmds::MapTPDO(1, 0x3A04, 1, MiControlCmds::DataSize32)); // Prêdkosc enkodera.
		sdo.PushCommand(MiControlCmds::EnableTPDO(2, 1));

		// TPDO 2
		sdo.PushCommand(MiControlCmds::DisableTPDO(2));
		sdo.PushCommand(MiControlCmds::SetTransmissionType(2));
		sdo.PushCommand(MiControlCmds::MapTPDO(2, 0x3262, 0, MiControlCmds::DataSize32)); // Pr¹d silnika.
		sdo.PushCommand(MiControlCmds::EnableTPDO(3, 1));

		sdo.StartSequence();
	}


	void SetVelocity(float value)
	{
		pdo.cmdTx[Pdo::Pdo1].data0 = (int32_t) value;
		pdo.Send(Pdo::Pdo1);
	}

	float ReadEncoder()
	{
		// Odczytywanie 2 z 4 bajtów pozycji enkodera.
		/*if (canDrv.dataRx[canDrv.indexRxStore].index == 0x181)
			if (Index < 1000) Angle[Index++] = canDrv.dataRx[canDrv.indexRxStore].data[0];
			else int a = 0;*/
	}

	float ReadVelocity()
	{
		/*if (canDrv.dataRx[canDrv.indexRxStore].index == 0x281)
			if (Index2 < 1000) Velocity[Index2++] = canDrv.dataRx[canDrv.indexRxStore].data[0];
			else int a = 0;*/
	}

	float ReadCurrent()
	{
		/*if (canDrv.dataRx[canDrv.indexRxStore].index == 0x381)
			if (Index3 < 1000) Current[Index3++] = canDrv.dataRx[canDrv.indexRxStore].data[0];
			else int a = 0;*/
	}


};
