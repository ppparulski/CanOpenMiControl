#pragma once

#include "can\can_drv.hpp"
#include "motor\mi_control.hpp"

inline bool motorCanSendData(MotorDrv * motor[], uint8_t motorNumbers)
{
   bool operational = false;
   for(auto m = 0; m < motorNumbers; m++)
   {
      switch (motor[m]->state)
	  {
	     case MotorDrv::Idle:
	        motor[m]->sdo.StackWriteUpdate();
	    	if (motor[m]->sdo.completed)
	    	    motor[m]->state = MotorDrv::Configured;
	     break;

	     case MotorDrv::Configured:
	        motor[m]->nmt.GoToOperational();
	    	motor[m]->state = MotorDrv::Waiting;
	     break;

	     case MotorDrv::Waiting:
	     {
	        bool ready = true;
	    	for(auto i = 0; i < motorNumbers; i++)
	    	   ready = ready && ((motor[i]->state == MotorDrv::Waiting)
	    							|| (motor[i]->state == MotorDrv::Operational));
			   if(ready)
			      motor[m]->state = MotorDrv::Operational;
	     }
	     break;

	     case MotorDrv::Operational:
	        motor[m]->SetCurrent();
	        operational = true;
		 break;
	  }
   }
   return operational;
}


inline void motorReadData(MotorDrv * mot, volatile CanMsg * msg, uint16_t cob)
{
   switch(cob)
   {
      case CanOpenObjects::sdoRx:
		 mot->sdo.received = true;
	  break;

	  case CanOpenObjects::pdo1Rx:
	     mot->ReadPosition(msg);
	  break;

	  case CanOpenObjects::pdo2Rx:
		 mot->ReadCurrent(msg);
      break;

	  case CanOpenObjects::pdo3Rx:
	  break;

	  case CanOpenObjects::pdo4Rx:
	  break;

	  default:
	  break;
   }
}
