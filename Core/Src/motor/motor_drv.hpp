#pragma once
#include "mi_control.hpp"
#include "string.h"
#include "math.h"


class MotorDrv
{
	typedef struct
	{
		uint32_t index;
		int32_t pos;
		int32_t vel;
	} __attribute__ ((packed)) ReceiveData1;

	typedef struct
	{
		uint32_t index;
		int16_t current;
		uint16_t status;
		uint32_t res;
	} __attribute__ ((packed)) ReceiveData2;

public:
	Sdo sdo;
	Pdo pdo;
	Nmt nmt;

	uint16_t id;

	enum State {Idle = 0, Configured = 1, Operational = 2, Waiting = 3};

	State state;

	float desiredVel;
	float desiredCurrent;

	int32_t measuredVel;
	int32_t measuredPos;
	int32_t measuredCurrent;
	int32_t actualStatus;
    float position;

	MotorDrv(CanDrv * canDrv, uint8_t id) : sdo(canDrv, id), pdo(canDrv, id), nmt(canDrv, id)
	{
		this->id = id;
		state = Idle;
		desiredVel = 0;
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

		/*sdo.PushCommand(MiControlCmds::DisableRPDO());
		sdo.PushCommand(MiControlCmds::MapRPDO(1, MiControlCmds::SetSubvel(0), MiControlCmds::DataSize32)); // Alternatywna sk�adnia: MapRPDO(1, 0x3500, 0, 32)
		sdo.PushCommand(MiControlCmds::EnableRPDO(1));



		//Map position and velocity to PDO 181
		sdo.PushCommand(MiControlCmds::DisableTPDO(0));
		sdo.PushCommand(MiControlCmds::SetTransmissionType(0));
		sdo.PushCommand(MiControlCmds::Map02hTxPDO(0, 0x3762, 0, MiControlCmds::DataSize32)); //position
		sdo.PushCommand(MiControlCmds::Map02hTxPDO(1, 0x3A04, 1, MiControlCmds::DataSize32)); //velocity
		sdo.PushCommand(MiControlCmds::EnableTPDO(1, 2)); //pdo 1 = (-1) + 0, 2 objects mapped

		//Map current and status to PDO 281
		sdo.PushCommand(MiControlCmds::DisableTPDO(1));
		sdo.PushCommand(MiControlCmds::SetTransmissionType(1));
		sdo.PushCommand(MiControlCmds::Map02hTxPDO(0, 0x3262, 0, MiControlCmds::DataSize32)); //current
		sdo.PushCommand(MiControlCmds::Map02hTxPDO(1, 0x3002, 1, MiControlCmds::DataSize32)); //status
		sdo.PushCommand(MiControlCmds::EnableTPDO(2, 2));*/



		// TPDO 0  //  181
		//sdo.PushCommand(MiControlCmds::DisableTPDO(0));
		//sdo.PushCommand(MiControlCmds::SetTransmissionType(0));
		//sdo.PushCommand(MiControlCmds::MapTPDO(0, 0x3762, 0, MiControlCmds::DataSize32)); // Pozycja enkodera (liczba impuls�w).
		//sdo.PushCommand(MiControlCmds::EnableTPDO(1, 1));

		// TPDO 1  //  281
		//sdo.PushCommand(MiControlCmds::DisableTPDO(1));
		//sdo.PushCommand(MiControlCmds::SetTransmissionType(1));
		//sdo.PushCommand(MiControlCmds::MapTPDO(1, 0x3A04, 1, MiControlCmds::DataSize32)); // Pr�dkosc enkodera.
		//sdo.PushCommand(MiControlCmds::EnableTPDO(2, 1));

		// TPDO 2  //  381
		//sdo.PushCommand(MiControlCmds::DisableTPDO(2));
		//sdo.PushCommand(MiControlCmds::SetTransmissionType(2));
		//sdo.PushCommand(MiControlCmds::MapTPDO(2, 0x3262, 0, MiControlCmds::DataSize32)); // Pr�d silnika.
		//sdo.PushCommand(MiControlCmds::EnableTPDO(3, 1));

		sdo.StartSequence();
	}


	void SetVelocity()
	{
		pdo.cmdTx[Pdo::Pdo1].data0 = (int32_t) desiredVel;
		pdo.Send(Pdo::Pdo1);
	}

	void SetCurrent()
	{
		pdo.cmdTx[Pdo::Pdo1].data0 = (int32_t) desiredCurrent;
		pdo.Send(Pdo::Pdo1);
	}

	float ReadPosition(volatile CanMsg * msg)
	{
		auto data = (ReceiveData1 *) msg;
		measuredPos = data->pos;
		measuredVel = data->vel;
		position = 2*M_PI*(float)measuredVel/(2000*53);
	}

	float ReadCurrent(volatile CanMsg * msg)
	{
		auto data = (ReceiveData2 *) msg;
		measuredCurrent = data->current;
	}

	float ReadStatus(volatile CanMsg * msg)
	{
		//measuredCurrent = (int32_t) msg->data[1];
	}

};
