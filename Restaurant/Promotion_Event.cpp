#include "Promotion_Event.h"
#include "Rest\Restaurant.h"


Promotion_Event::Promotion_Event(int eTime, int oID, double Ex_Mon):Event(eTime, oID)
{
	EXMoney = Ex_Mon;
}

void Promotion_Event::Execute(Restaurant* pRest)
{
	pRest->Promote_Normal(OrderID,EXMoney);
}

