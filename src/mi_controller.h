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

	SdoCmd * DisableRPDO()
	{
		cmd.type = 0x23;
		cmd.index = 0x1600;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = 0;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	SdoCmd * MapRPDO(int cmdCode)
	{
		cmd.type = 0x23;
		cmd.index = 0x1600;
		cmd.subindex = 0x01;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = cmdCode;
		cmd.data[1] = cmdCode >> 8;
		cmd.data[2] = cmdCode >> 16;
		cmd.data[3] = cmdCode >> 24;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

	SdoCmd * EnableRPDO(int Number)
	{
		cmd.type = 0x23;
		cmd.index = 0x1600;
		cmd.subindex = 0x00;
		*(uint32_t*)cmd.data = 0;
		cmd.data[0] = Number;
		cmd.timeout = 10;
		cmd.trials = -1;
		return &cmd;
	}

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
