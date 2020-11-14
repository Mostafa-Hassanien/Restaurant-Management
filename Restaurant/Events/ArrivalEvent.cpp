#include "ArrivalEvent.h"
#include "..\Rest\Restaurant.h"


ArrivalEvent::ArrivalEvent(int eTime, int oID, ORD_TYPE oType):Event(eTime, oID)
{
	OrdType = oType;
}
ArrivalEvent::ArrivalEvent(int eTime, int oID, ORD_TYPE oType,double OrdMONY,int ordSize): Event(eTime,oID)
{
	OrdType = oType;
	OrderSize = ordSize;
	OrdMoney = OrdMONY;
}

void ArrivalEvent::Execute(Restaurant* pRest)
{
	Order* pOrd = new Order(OrderID,OrdType,EventTime,OrderSize,OrdMoney);
	switch (OrdType)
	{
	case TYPE_NRM:
		pRest->AddtoNORMALQueue(pOrd);
		break;
	case TYPE_VGAN:
		pRest->AddtoVEGANQueue(pOrd);
		break;
	case TYPE_VIP:
		pRest->AddtoVIPQueue(pOrd);
		break;
	}
}
