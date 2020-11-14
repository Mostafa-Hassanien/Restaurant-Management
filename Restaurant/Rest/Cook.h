#pragma once

#include "..\Defs.h"
#include "Order.h"
#include <string> 
using namespace std;
#pragma once
class Cook
{
	int ID;
	ORD_TYPE type;	//for each cook type there is a corresponding type (VIP, Normal, Vegan)
	int speed;		//dishes it can prepare in one clock tick (in one timestep)
	int break_duration;  //Each cook takes a break after serving n consecutive orders(in timesteps)
	int RstPrd; // the necessary rest period for injured cook to have the necessary medication
	int RstPrd_Duration; // the duration of rest period already taken by the cook till this time step 
	int number_order; // the number of orders a cook must prepare before taking a break
	Order* pOrder; // Order Pointer to point to the assigned order to this cook
	Cook_Status Assigned_Status; // for each cook there is a different status during the simulation 
	                             //  1. Assigned: assigned an order
								 //  2. Assigned_Urgently: assigned an urgent order 
								 //  3. Not_Assigned: is available  
								 //  4. Injured: an injured busy cook
								 //  5. In_Break: taking his/her break 
								 //  6. In_Rest: an injured cook is taking his/her rest
	int Num_OrdDone; // the number of orders done from the break_duration till the current time step 
	double Size_OrderReq; // the size of the required order to be finished 
	int Break_StartTime; // the starting time of the break 

public:
	Cook();
	virtual ~Cook();
	int GetID() const;
	ORD_TYPE GetType() const;
	void setID(int);
	void setType(ORD_TYPE) ;
	void setSpeed(int);
	void setBreak(int);
	void set_NumOrder(int);
	int GetSpeed() const;
	int GetBreak() const;
	int Get_NumOrder() const;
	void Assign_Order(Order* pO, int CurrentStep); // assign the passed pointer order to the given cook 
	void Assign_UrgentOrder(Order* pO, int CurrentStep); //  assign the passed pointer urgent order to the given cook 
	Cook_Status get_Assigned() const;
	string get_AssignedOrd_Info();
	Order* Advance_AssignedOrd(int CurrentTimeStep); // advance the assigned order at each timestep till the order is finished
	int Avail_Time() const; // returns the availbility time of the given cook by subtracting the number of order done till this timestep 
	                        // from number of order required before taking the break 
	int Get_Num_OrdDone();
	void Set_Assigned_Status(Cook_Status);
	int Get_Break_StartTime() const;
	int Get_RstPrd_Duration() const;
	void Advance_BreakDuration(); // advance the break duration of the cook to change its status to be availble after finishing the brak duration
	void Advance_RstPrd_Duration(); //  advance the rest period of the injured cook to change its status to be availble after finishing his/her rest
	void set_RstPrd( int t);
	int get_RstPrd() const;
};
