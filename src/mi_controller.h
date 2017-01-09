#pragma once

#include "sdo.h"

class MiControlCmds
{

public:

	SdoCmd cmd;

	SdoCmd * ClearError()
	{
		cmd.type = 0x23;
		//cmd.index = __builtin_bswap16(0x3000);
		cmd.index = 0x3000;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = 0x01;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	// numberOfBits: Only 8, 16 and 32 available.
	SdoCmd * MapRPDO(int object, int index, int subIndex, int numberOfBits)
	{
		cmd.type = 0x23;
		cmd.index = 0x1600;
		cmd.subindex = object;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = numberOfBits;
		cmd.data[1] = subIndex;
		cmd.data[2] = index;
		cmd.data[3] = index >> 8;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	// Alternatywna sk³adnia mapowania RPDO.
	SdoCmd * MapRPDO(int object, SdoCmd* Command, int numberOfBits)
	{
		return MapRPDO(object, Command->index, Command->subindex, numberOfBits);
	}

	SdoCmd * EnableRPDO(int numberOfObjects)
	{
		cmd.type = 0x23;
		cmd.index = 0x1600;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = numberOfObjects;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}
	SdoCmd * DisableRPDO() { EnableRPDO(0); }


	SdoCmd * TransmissionType()
	{
		cmd.type = 0x23;
		cmd.index = 0x1800;
		cmd.subindex = 0x02;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = 0x01;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	// numberOfBits: Only 8, 16 and 32 available.
	SdoCmd * MapTPDO(int object, int index, int subIndex, int numberOfBits)
	{
		cmd.type = 0x23;
		cmd.index = 0x1A00;
		cmd.subindex = object;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = numberOfBits;
		cmd.data[1] = subIndex;
		cmd.data[2] = index;
		cmd.data[3] = index >> 8;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	// Alternatywna sk³adnia mapowania RPDO.
	SdoCmd * MapTPDO(int object, SdoCmd* Command, int numberOfBits)
	{
		return MapRPDO(object, Command->index, Command->subindex, numberOfBits);
	}

	SdoCmd * EnableTPDO(int numberOfObjects)
	{
		cmd.type = 0x23;
		cmd.index = 0x1A00;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = numberOfObjects;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}
	SdoCmd * DisableTPDO() { EnableTPDO(0); }

	SdoCmd * NMTOperational()
	{
		cmd.type = 0x01;
		cmd.index = 0x0001;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}



	SdoCmd * LoadDefault()
	{
		cmd.type = 0x23;
		cmd.index = 0x3000;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = 0x82;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}
	SdoCmd * RestoreParam()
	{
		cmd.type = 0x23;
		cmd.index = 0x3000;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = 0x81;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	SdoCmd * SetMotorDC()
	{
		cmd.type = 0x23;
		cmd.index = 0x3900;
		cmd.subindex = 0x00;
		cmd.data[0] = 0x00;
		cmd.timeout = 20;
		cmd.trials = -1;
		return &cmd;
	}

	SdoCmd * SetMotorBLDC()
	{
		cmd.type = 0x23;
		cmd.index = 0x3900;
		cmd.subindex = 0x00;
		cmd.data[0] = 0x01;
		cmd.timeout = 20;
		cmd.trials = -1;
		return &cmd;
	}

	SdoCmd * MotorEnable()
	{
		cmd.type = 0x23;
		cmd.index = 0x3004;
		cmd.subindex = 0x00;
		cmd.data[0] = 0x01;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	SdoCmd * MotorDisable()
	{
		cmd.type = 0x23;
		cmd.index = 0x3004;
		cmd.subindex = 0x00;
		cmd.data[0] = 0x00;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}


	SdoCmd * SetSubvel(int32_t value)
	{
		cmd.type = 0x23;
		cmd.index = 0x3500;
		cmd.subindex = 0x00;

		// Is it possible to use something like this: "*(uint32_t*)cmd.data = value" ?
		cmd.data[0] = value;
		cmd.data[1] = value >> 8;
		cmd.data[2] = value >> 16;
		cmd.data[3] = value >> 24;
		cmd.timeout = 5;
		cmd.trials = -1;
		return &cmd;
	}

};
