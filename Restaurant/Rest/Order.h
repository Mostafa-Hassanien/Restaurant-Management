#ifndef __ORDER_H_
#define __ORDER_H_

#include "..\Defs.h"

class Order
{

protected:
	int ID;         //Each order has a unique ID (from 1 --> 999 )
	ORD_TYPE type;		//order type: Normal, vegan, VIP
	ORD_STATUS status;	//waiting, in-service, done
	int Distance;	//The distance (in meters) between the order location and the resturant              
	double totalMoney;	//Total order money
	int ArrTime, ServTime, FinishTime;	//arrival, service start, and finish times
	int order_size; //  the number of dishes for this order (in dishes)

public:
	Order(int ID, ORD_TYPE r_Type,int ArrT, int ord_size, double Mon );
	virtual ~Order();
	int GetID() const;
	ORD_TYPE GetType() const;
	void SetDistance(int d);
	int GetDistance() const;
	void setStatus(ORD_STATUS s);
	ORD_STATUS getStatus() const;
	int get_ArrTime() const;
	int get_size() const;
	double get_Money() const;
	void SetMoney(double MNY);
	void setType(ORD_TYPE t);
	void setSize(int t);
	void Set_FinishedTime(int t);
	void Set_ServeTime(int t);
	int getServeTime() const;
	int get_FinishTime() const;
};
#endif