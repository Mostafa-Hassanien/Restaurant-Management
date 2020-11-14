#include "Order.h"

Order::Order(int id, ORD_TYPE r_Type,int ArrT, int ord_size,  double Mon )
{
	ID = (id>0&&id<1000)?id:0;	//1<ID<999
	type = r_Type;
	status = WAIT;
	ArrTime = ArrT;
	order_size = ord_size;
	totalMoney = Mon;
}

Order::~Order()
{
}

int Order::GetID() const
{
	return ID;
}


ORD_TYPE Order::GetType() const
{
	return type;
}

int Order::get_FinishTime() const
{
	return FinishTime;
}
void Order::SetDistance(int d)
{
	Distance = d>0?d:0;
}

int Order::GetDistance() const
{
	return Distance;
}

void Order::setSize(int t)
{
	order_size = t;
}

void Order::setStatus(ORD_STATUS s)
{
	status = s;
}

void Order::setType(ORD_TYPE t)
{
	type = t;
}
ORD_STATUS Order::getStatus() const
{
	return status;
}

int Order::get_ArrTime() const
{
	return ArrTime;
}
int Order::get_size() const
{
	return order_size;
}
double Order::get_Money() const
{
	return totalMoney;
}

void Order::SetMoney(double MNY)
{
	totalMoney = MNY;
}

void Order::Set_FinishedTime(int t)
{
	FinishTime = t;
}
void Order::Set_ServeTime(int t)
{
	ServTime = t;
}
int Order::getServeTime() const
{
	return ServTime;
}