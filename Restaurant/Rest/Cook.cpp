#include "Cook.h"


Cook::Cook()
{
	Assigned_Status = Not_Assigned;
	pOrder = nullptr;
	Num_OrdDone = 0;
	Size_OrderReq = 0;
	Break_StartTime = 0;
	RstPrd_Duration = 0;
}


Cook::~Cook()
{
}


int Cook::GetID() const
{
	return ID;
}

int Cook::Avail_Time() const
{
	return number_order - Num_OrdDone;
}

ORD_TYPE Cook::GetType() const
{
	return type;
}


void Cook::setID(int id)
{
	ID = id;
}

void Cook::setType(ORD_TYPE t)
{
	type = t;
}

void Cook::setSpeed(int t)
{
	speed = t;
}
void Cook::setBreak(int t)
{
	break_duration = t;
}

void Cook::Set_Assigned_Status(Cook_Status status)
{
	Assigned_Status = status;
	if ( status == In_Break )
	{
		Break_StartTime = 0;
		Num_OrdDone = 0;
		RstPrd_Duration =0;
	}
}

int Cook::Get_Break_StartTime() const
{
	return Break_StartTime;
}
int Cook::Get_RstPrd_Duration() const
{
	 return RstPrd_Duration;
}
void Cook::Advance_RstPrd_Duration()
{
	RstPrd_Duration++;
}
void Cook::set_NumOrder(int t)
{
	number_order = t;
}
int Cook::GetSpeed() const
{
	return speed;
}
int Cook::GetBreak() const
{
	return break_duration;
}
int Cook::Get_NumOrder() const
{
	return number_order;
}

void Cook::Advance_BreakDuration()
{
	Break_StartTime++;
}

int Cook::Get_Num_OrdDone()
{
	return Num_OrdDone;
}

void Cook::Assign_Order(Order* pO,int CurrentStep)
{
	pOrder = pO;
	pOrder ->Set_ServeTime(CurrentStep);
	Assigned_Status = Assigned;
	Size_OrderReq = pO->get_size();
}
void Cook::Assign_UrgentOrder(Order* pO, int CurrentStep)
{
	pOrder = pO;
	pOrder ->Set_ServeTime(CurrentStep);
	Assigned_Status = Assigned_Urgently;
	Size_OrderReq = pO->get_size();
}


Cook_Status Cook::get_Assigned() const
{
	return  Assigned_Status;
}

Order* Cook::Advance_AssignedOrd(int CurrentTimeStep)
{
	if( Assigned_Status == Assigned )
	{
			if ( Size_OrderReq > speed)
			{
				Size_OrderReq = Size_OrderReq - speed;
				return nullptr;
			}
		else 
			{
				pOrder ->setStatus(DONE);
				pOrder ->Set_FinishedTime(CurrentTimeStep);
				Num_OrdDone ++;
				Size_OrderReq = 0;
				Assigned_Status = Not_Assigned;
				return pOrder;
			}
	}
	else 
	{
			if ( Size_OrderReq > (speed/2.0))
			{
				Size_OrderReq = Size_OrderReq - (speed/2.0);
				return nullptr;
			}
		else 
			{
				pOrder ->setStatus(DONE);
				pOrder ->Set_FinishedTime(CurrentTimeStep);
				Num_OrdDone ++;
				Size_OrderReq = 0;
				if (Assigned_Status == Assigned_Urgently)
					Assigned_Status = Not_Assigned;
			   else
			   {
				   Assigned_Status = In_Rest;
				   RstPrd_Duration = 0;
				   Num_OrdDone = 0;
				}
				return pOrder;
			}
	}
}

string Cook::get_AssignedOrd_Info()
{
	string msg = "No";
	if ( pOrder )
	{
		if( Size_OrderReq == pOrder->get_size())
		{
			if(type == TYPE_NRM)
				msg = "N" + to_string(ID);
			else if(type == TYPE_VIP)
				msg = "V" + to_string(ID);
			else
				msg = "G" + to_string(ID);
			if (pOrder->GetType() == TYPE_NRM)
				msg += "(N" + to_string(pOrder->GetID()) +")";
			else if(pOrder->GetType() == TYPE_VIP)
				msg += "(V" + to_string(pOrder->GetID()) +")";
			else
				msg += "(G" + to_string(pOrder->GetID()) +")";
		}
	}
	return msg;
}

void Cook::set_RstPrd( int t)
{
	RstPrd = t;
}
int Cook::get_RstPrd() const
{
	return RstPrd;
}
