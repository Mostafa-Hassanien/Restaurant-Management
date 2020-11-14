#ifndef __PRO_EVENT_H_
#define __PRO_EVENT_H_

#include "Events\Event.h"

class Promotion_Event : public Event
{
	double EXMoney;	//extra Money
public:
	Promotion_Event(int eTime, int oID, double EX_Mon);
	
	virtual void Execute(Restaurant *pRest);	//override execute function

};


#endif
