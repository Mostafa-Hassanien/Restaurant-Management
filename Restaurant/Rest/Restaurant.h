#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include "..\Defs.h"
#include "..\CMUgraphicsLib\CMUgraphics.h"
#include "..\GUI\GUI.h"
#include "..\Generic_DS\Queue.h"
#include "..\Events\Event.h"
#include <fstream>
#include "..\List.h"
#include "Order.h"
#include "..\PRTQueue.h"
// it is the maestro of the project
class Restaurant  
{	
private:
	GUI *pGUI;
	Queue<Event*> EventsQueue;	//Queue of all events that will be loaded from file
	List<Order*> Normal_Orders; // list of all Normal Orders 
	List<Order*> VIP_Orders; // List of all VIP Orders
	Queue<Order*> Vegan_Orders; //  Queue of all Vegan Orders 
	List<Order*> InService_Orders; // list of all inservice Orders
	Queue<Order*> Finished_Orders; // list of all inservice Orders
	List<Order*> Urgent_Order; // list of all urgent orders
	List<Cook*> Normal_Cooks; // list of all Normal Cooks 
	List<Cook*> VIP_Cooks; // list of all VIP Cooks 
	List<Cook*> Vegan_Cooks; // list of all Vegan Cooks 
	List<Cook*> InRest_Cook; // list of all cooks in rest due to injury 
	int AutoPromotion; //  represent the number of timesteps after which an order is automatically promoted to VIP
	int Auto_promoted; // number of Auto-promoted orders
	int Num_NormalCook; // number of normal cooks
	int Avail_CookN; // number of available Normal cooks
	int Num_VIPCook; // number of VIP cooks  
	int Avail_CookV; // number of available VIP cooks
	int Num_VeganCook; // number of Vegan cooks
	int Avail_CookG; // number of available vegan cooks
	int Num_FinishedN; // number of finished normal orders
	int Num_FinishedG; // number of finished Vegan orders
	int Num_FinishedV; // number of finished VIP orders
	int Num_WaitingN; // number of Waiting normal orders at the given time step
	int Num_WaitingG; // number of Waiting Vegan orders at the given time step
	int Num_WaitingV; // number of Waiting VIP orders at the given time step
	int VIP_WT; // the number of ticks after which a VIP order is considered urgent
	int Num_UrgentOrders; // the number of urgent orders 
	double InjProp; // the probability of a busy cook to be injured
	int Num_InjuredCooks; // the total number of injured cooks during the simulation 
public:
	
	Restaurant();
	~Restaurant();
	
/// ================================================================================================== 
///					          Simulation Handling Functions 		
/// ==================================================================================================
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation(); 
/// ==================================================================================================

/// ================================================================================================== 
///					          Arrival Event Handling Functions 		
/// ==================================================================================================
	void AddtoVIPQueue(Order* pOrd); //adds an order to the VIP queue
	double Calc_Priority(Order* pOrd); // claculate the priority of the VIP orders
	void AddtoNORMALQueue(Order* pOrd); //adds an order to the NORMAL queue
	void AddtoVEGANQueue(Order* pOrd); //adds an order to the VEGAN queue
	void Cancel_Normal(int OrderID); // cancel the normal order 
	void Promote_Normal(int OrderID,double Money); // promote the normal order
/// ==================================================================================================

/// ================================================================================================== 
///					          Input File Loading Handling Functions 		
/// ==================================================================================================
	void Load_InfoFile(ifstream& Infile); // load the information of both cooks' and events' data
	void Read_CookInfo(ifstream& Infile); // read the information of every cook from the input file
	void Read_Events(ifstream& Infile); // read the informatin of all events from the input file
/// ==================================================================================================

/// ================================================================================================== 
///					          Output File Handling Functions 		
/// ==================================================================================================
	void Output_File(); // produce the output file
	Order**  Sorted_FinishedOrders(Queue<Order*> pO, int& count); // sort Finished orders by finished time in ascending order. If more than one 
	                                                             // order is finished at the same timestep, they are ordered by service time
/// ==================================================================================================

/// ================================================================================================== 
///					          Printing Information and Drawing Data Handling Functions 		
/// ==================================================================================================
	void FillDrawingList(); // Draws all the required Information at each timestep (IDs of available cooks and waiting, serving, and finished orders)
	string Print_Info_1(int TimeStep); // returns the required info at the bottom of the screen in each timestep (First Line)
	string Print_Info_2(); // returns  required info at the bottom of the screen in each timestep (Second Line)
	string Print_Info_3(); // returns  required info at the bottom of the screen in each timestep (Third Line)
	string Print_Info_4(); // returns  required info at the bottom of the screen in each timestep (Fourth Line)
	string Print_Info_5(); // returns  required info at the bottom of the screen in each timestep (Fifth Line)
	Cook** Sort_Cook(int & Count); // sort the cooks according to their  availability time
	void All_AvailCook(); // make all cooks available after the simulation has already finished 
/// ==================================================================================================

/// ================================================================================================== 
///					          Assigned Orders Handling Functions 		
/// ==================================================================================================
	void Assign_Orders(int CurrentStep); // assigning orders to ccoks 
	int Get_AvailableCook(List<Cook*> & Co); // return the index of the available cook
	void Advance_Order(int CurrentTimeStep); // advance the preparation of the assigned order
	void Auto_PromoteNormal(int CurrentTimeStep); // Auto-Promotion for Normal not-assigned Orders 
/// ==================================================================================================

/// ================================================================================================== 
///					          Urgent Orders Handling Functions 		
/// ==================================================================================================
	void Urgent_Orders(int CurrentTimeStep); //  assign Urgent Orders 
    void Insert_UrgentOrders(int CurrentTimeStep); // check in VIP list for Urgent Orders that waited longer than VIP_WT and insert them 
	                                              // in Urgent-Orders list 
/// ================================================================================================== 

/// ================================================================================================== 
///					          Cooks Handling Functions 		
/// ==================================================================================================
	int Get_FirstBusyCook(List<Cook*> & Co); // return the index of the first busy cook
	int Get_InBreakCook(List<Cook*> & Co); // return the index of the in-break or in-rest cook
	void Break_Time(); // break time for specific cooks
	void Return_Break(); // finished time of the break 
	void Return_Rest(); // finished time for the rest period of injured cooks
	void Injured_BusyCook(); // generates a random number R and if R ≤ InjProp, then it picks the first busy cook (if any) and make him/her injured.
/// ================================================================================================== 
};

#endif