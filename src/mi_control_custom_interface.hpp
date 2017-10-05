
	void Send(int Value)
	{
		canDrv->dataTx[0].index = 0x201;
		canDrv->dataTx[0].data[0] = Value;
		canDrv->dataTx[0].dataNumber = 4;

		canDrv->SetWrData();
		canDrv->SendTrigger();
	}
