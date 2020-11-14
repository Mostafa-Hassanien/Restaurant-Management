#include "Cancellation_Event.h"
#include "Rest\Restaurant.h"


Cancellation_Event::Cancellation_Event(int eTime, int oID):Event(eTime, oID)
{
	
}

void Cancellation_Event::Execute(Restaurant *pRest)
{
	pRest->Cancel_Normal(OrderID);
}