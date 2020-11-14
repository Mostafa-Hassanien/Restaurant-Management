#ifndef __CAN_EVENT_H_
#define __CAN_EVENT_H_

#include "Events\Event.h"

class Cancellation_Event : public Event
{
	// no data members 
public:
	Cancellation_Event(int eTime, int oID);
	
	virtual void Execute(Restaurant *pRest);	//override execute function
};
#endif
