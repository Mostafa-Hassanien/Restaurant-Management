#include <cstdlib>
#include <time.h>
#include <iostream>
using namespace std;

#include "Restaurant.h"
#include "..\Events\ArrivalEvent.h"
#include <fstream>
#include "..\Promotion_Event.h"
#include "..\Cancellation_Event.h"

Restaurant::Restaurant() 
{
	pGUI = NULL;
	Num_FinishedN = 0;
	Num_FinishedG = 0;
	Num_FinishedV = 0;
	Auto_promoted = 0;
	Num_UrgentOrders = 0;
	Num_InjuredCooks = 0;
}

Restaurant::~Restaurant()
{
		if (pGUI)
			delete pGUI;
}

/// ================================================================================================== 
///					          Simulation Handling Functions 		
/// ==================================================================================================
void Restaurant::ExecuteEvents(int CurrentTimeStep)
{
	Event *pE;
	while( EventsQueue.peekFront(pE) )	//as long as there are more events
	{
		if(pE->getEventTime() > CurrentTimeStep )	//no more events at current timestep
			return;

		pE->Execute(this);
		EventsQueue.dequeue(pE);	//remove event from the queue
		delete pE;		//deallocate event object from memory
	}
}
/// ==================================================================================================
void Restaurant::RunSimulation()
{
	pGUI = new GUI;
	PROG_MODE	mode = pGUI->getGUIMode();
		srand(time(NULL));
		if ( mode == MODE_INTR)
		{
			pGUI->PrintMessage("This is the interactive Mode.Click to continue ...",true,1.03);
			pGUI->waitForClick();
		}
		else if ( mode == MODE_STEP)
		{
			pGUI->PrintMessage("This is the Step-By-Step Mode.Click to continue ...",true,1.03);
			pGUI->waitForClick();
		}
		else
		{
			pGUI->PrintMessage("This is the Silent Mode.Click to continue ...",true,1.03);
			pGUI->waitForClick();
		}
		pGUI->PrintMessage("Please, Enter the file Name: ",true,1.03);
		string FileName; 
		FileName = pGUI->GetString();
		ifstream Infile;
		string file = FileName + ".txt";
		Infile.open(file);
		if ( !Infile.is_open())
		{
			pGUI ->PrintMessage(" This File doesn't exist. Click to continue ... ",true,1.03);
			pGUI ->waitForClick();
			return;
		}
		Load_InfoFile(Infile);
		int CurrentTimeStep = 1;
		while(!EventsQueue.isEmpty() || !InService_Orders.Is_Empty() || !Normal_Orders.Is_Empty() || !VIP_Orders.Is_Empty() || !Vegan_Orders.isEmpty())
		{
				string msg_1;
				ExecuteEvents(CurrentTimeStep);
				Urgent_Orders(CurrentTimeStep);
				Auto_PromoteNormal(CurrentTimeStep);
				Return_Break();
				Return_Rest();
				Assign_Orders(CurrentTimeStep);
				if ( mode == MODE_INTR || mode == MODE_STEP)
				{
					FillDrawingList();
					msg_1 = Print_Info_1(CurrentTimeStep);
					pGUI->PrintMessage(msg_1,false,1.03);
					msg_1 = Print_Info_2();
					pGUI->PrintMessage(msg_1,false,1.3);
					msg_1 = Print_Info_3();
					pGUI->PrintMessage(msg_1,false,1.7);
					msg_1 = Print_Info_4();
					pGUI->PrintMessage(msg_1,false,2.4);
					msg_1 = Print_Info_5();
					pGUI->PrintMessage(msg_1,false,4.1);
					pGUI->UpdateInterface();
				}
				if ( mode == MODE_INTR )
					pGUI->waitForClick();
				else if ( mode == MODE_STEP)
					Sleep(1000);
				CurrentTimeStep++;	//advance timestep
				Advance_Order(CurrentTimeStep);
				Break_Time();
				Injured_BusyCook();
				if ( mode == MODE_INTR || mode == MODE_STEP)
				{
					pGUI ->PrintMessage(msg_1,true,0);
					pGUI->ResetDrawingList();
				}
		}
		All_AvailCook();
		if ( mode == MODE_INTR || mode == MODE_STEP)
		{
			FillDrawingList();
			pGUI->UpdateInterface();
		}
		if ( mode == MODE_SLNT ) 
			  Sleep(3000);
		if ( mode == MODE_INTR)
			pGUI->PrintMessage("Interactive Mode is finished, click to END program",true,1.03);
		else if ( mode == MODE_STEP)
			pGUI->PrintMessage("Step-By-Step Mode is finished, click to END program",true,1.03);
		else
			pGUI->PrintMessage("Silent Mode is finished, click to END program",true,1.03);
		pGUI->waitForClick();
		Output_File();
}
/// ==================================================================================================
/// ==================================================================================================

/// ================================================================================================== 
///					          Arrival Event Handling Functions 		
/// ==================================================================================================
void Restaurant::AddtoVIPQueue(Order* pOrd)
{
	
	VIP_Orders.Insert_Priority(Calc_Priority(pOrd),pOrd);
}
/// ==================================================================================================
double Restaurant::Calc_Priority(Order* pOrd)
{
	double size = pOrd->get_size();
	double arriveTime = pOrd->get_ArrTime();
	double money = pOrd->get_Money();
	return (0.3*size) + (0.3*arriveTime) + (0.4*money);
}
/// ==================================================================================================
void Restaurant::AddtoNORMALQueue(Order* pOrd)
{
	Normal_Orders.Insert(Normal_Orders.Get_Length(),pOrd);
}
/// ==================================================================================================
void Restaurant::AddtoVEGANQueue(Order* pOrd)
{
	Vegan_Orders.enqueue(pOrd);
}
/// ==================================================================================================
void Restaurant::Cancel_Normal(int OrderID)
{
	Order* pOrder;
	if ( Normal_Orders.Get_EntryID(OrderID,pOrder) )
	{
		Normal_Orders.Remove_Entry(pOrder);
	}
}
/// ==================================================================================================
void Restaurant::Promote_Normal(int OrderID, double Money)
{
	Order* pOrder;
	if ( Normal_Orders.Get_EntryID(OrderID,pOrder) )
	{
		pOrder ->SetMoney(Money + pOrder ->get_Money());
		Normal_Orders.Remove_Entry(pOrder);
		pOrder ->setType(TYPE_VIP);
		VIP_Orders.Insert_Priority(Calc_Priority(pOrder),pOrder);
	}
}
/// ==================================================================================================
/// ==================================================================================================

/// ================================================================================================== 
///					          Input File Loading Handling Functions 		
/// ==================================================================================================
void Restaurant::Load_InfoFile(ifstream& Infile)
{
	Read_CookInfo(Infile);
	Infile >> AutoPromotion;
	Infile >> VIP_WT;
	Read_Events(Infile);
}
/// ==================================================================================================
void Restaurant::Read_CookInfo(ifstream& Infile)
{
	Infile >> Num_NormalCook;
	Infile >> Num_VeganCook;
	Infile >> Num_VIPCook;
	int SN_min;
	int SN_max;
	Infile >> SN_min;
	Infile >> SN_max;
	int SG_min; 
	int SG_max;
	Infile >> SG_min;
	Infile >> SG_max;
	int SV_min; 
	int SV_max;
	Infile >> SV_min;	
	Infile >> SV_max;
	int BO;
	int BN_min;
	int BN_max;
	int BG_min; 
	int BG_max;
	int BV_min; 
	int BV_max;
	Infile >> BO;
	Infile >> BN_min;
	Infile >> BN_max;	
	Infile >> BG_min;
	Infile >> BG_max;	
	Infile >> BV_min;	
	Infile >> BV_max;
	int RstPrd;
	Infile >> InjProp;
	Infile >> RstPrd;
	int cID = 1;
	for ( int i = 0; i < Num_NormalCook ; i++)
	{
		Cook* pCook = new Cook();
		pCook ->setType(TYPE_NRM);
		cID+= (rand()%15+1);
		pCook ->setID(cID);
		int random = SN_max - SN_min + 1;
		int SN = (rand()%random) + SN_min;
		pCook ->setSpeed(SN);
		pCook ->set_NumOrder(BO);
		random = BN_max - BN_min + 1;
		int BN = (rand()%random) + BN_min;
		pCook ->setBreak(BN);
		pCook->set_RstPrd(RstPrd);
		Normal_Cooks.Insert(i,pCook);
	}
	for ( int i = 0; i < Num_VeganCook ; i++)
	{
		Cook* pCook = new Cook();
		pCook ->setType(TYPE_VGAN);
		cID+= (rand()%15+1);
		pCook ->setID(cID);
		int random = SG_max - SG_min + 1;
		int SG = (rand()%random) + SG_min;
		pCook ->setSpeed(SG);
		pCook ->set_NumOrder(BO);
		random = BG_max - BG_min + 1;
		int BG = (rand()%random) + BG_min;
		pCook ->setBreak(BG);
		pCook->set_RstPrd(RstPrd);
		Vegan_Cooks.Insert(i,pCook);
	}
	for ( int i = 0; i < Num_VIPCook ; i++)
	{
		Cook* pCook = new Cook();
		pCook ->setType(TYPE_VIP);
		cID+= (rand()%15+1);
		pCook ->setID(cID);
		int random = SV_max - SV_min + 1;
		int SV = (rand()%random) + SV_min;
		pCook ->setSpeed(SV);
		pCook ->set_NumOrder(BO);
		random = BV_max - BV_min + 1;
		int BV = (rand()%random) + BV_min;
		pCook ->setBreak(BV);
		pCook->set_RstPrd(RstPrd);
		VIP_Cooks.Insert(i,pCook);
	}
}
/// ==================================================================================================
void Restaurant::Read_Events(ifstream& Infile)
{
	Event* pEv;
	int Num_Events;
	int TS;
	int ID;
	int SIZE;
	double MONY;
	Infile >> Num_Events;
	for ( int i= 0 ; i < Num_Events ; i ++)
	{
		char Type_Event;
		Infile >> Type_Event;
		switch (Type_Event)
		{
			case 'R':
				char TYP;
				Infile >> TYP;
				Infile >> TS;
				Infile >> ID;
				Infile >> SIZE;
				Infile >> MONY;
					switch (TYP)
					{
						case 'N':
							pEv = new ArrivalEvent(TS,ID,TYPE_NRM,MONY,SIZE);
							break;
						case 'G':
							pEv = new ArrivalEvent(TS,ID,TYPE_VGAN,MONY,SIZE);
							break;
						case 'V' :
							pEv = new ArrivalEvent(TS,ID,TYPE_VIP,MONY,SIZE);
							break;
					}
				EventsQueue.enqueue(pEv);
				break;
			case 'X' :
				Infile >> TS;
				Infile >> ID;
				pEv = new Cancellation_Event(TS,ID);
				EventsQueue.enqueue(pEv);
				break;
			case 'P' :
				Infile >> TS;
				Infile >> ID;
				Infile >> MONY;
				pEv = new Promotion_Event(TS,ID,MONY);
				EventsQueue.enqueue(pEv);
				break;
		}
	}
}
/// ==================================================================================================
/// ==================================================================================================

/// ================================================================================================== 
///					          Output File Handling Functions 		
/// ==================================================================================================
void Restaurant::Output_File()
{
	pGUI ->PrintMessage("Now, it's time to produce Output File, Click to Continue...",true,1.03);
	pGUI->waitForClick();
	pGUI ->PrintMessage(" Enter the file name, please: ",true,1.03);
	string FileName = pGUI->GetString();
	ofstream OutFile;
	string file = FileName + ".txt";
	OutFile.open(file);
	int sum_1 = 0;
	int sum_2 = 0;
	int count = 0;
	OutFile << "FT" << "   " << "ID" << "   " << "AT" << "   " << "WT" << "   " << "ST" << endl;
	string Space;
	Order** pO = Sorted_FinishedOrders(Finished_Orders, count);
	for ( int i =0; i < count ; i++ )
	{
		OutFile << pO[i]->get_FinishTime();
		if ( pO[i]->get_FinishTime() >= 0 && pO[i]->get_FinishTime() < 10 )
				Space = "    ";
		else if ( pO[i]->get_FinishTime() >= 10 && pO[i]->get_FinishTime() <= 99 )
			Space = "   ";
		else 
			Space = "  ";
		OutFile << Space;
		OutFile << pO[i]->GetID(); 
		if ( pO[i]->GetID() >= 0 && pO[i]->GetID() < 10 )
				Space = "    ";
		else if ( pO[i]->GetID() >= 10 && pO[i]->GetID() <= 99 )
			Space = "   ";
		else 
			Space = "  ";
		OutFile << Space;
		OutFile <<	pO[i]->get_ArrTime(); 
		if ( pO[i]->get_ArrTime() >= 0 && pO[i]->get_ArrTime() < 10 )
				Space = "    ";
		else if ( pO[i]->get_ArrTime() >= 10 && pO[i]->get_ArrTime() <= 99 )
			Space = "   ";
		else 
			Space = "  ";
		OutFile << Space;
		sum_1 += pO[i]->getServeTime() - pO[i]->get_ArrTime(); 
		OutFile	<< pO[i]->getServeTime() - pO[i]->get_ArrTime();
		if ( pO[i]->getServeTime() - pO[i]->get_ArrTime() >= 0 && pO[i]->getServeTime() - pO[i]->get_ArrTime() < 10 )
				Space = "    ";
		else if ( pO[i]->getServeTime() - pO[i]->get_ArrTime() >= 10 && pO[i]->getServeTime() - pO[i]->get_ArrTime() <= 99 )
			Space = "   ";
		else 
			Space = "  ";
		OutFile << Space;
		sum_2 += pO[i]->get_FinishTime() - pO[i]->getServeTime();
		OutFile	<< pO[i]->get_FinishTime() - pO[i]->getServeTime() << endl;
	}
	OutFile << "Orders: " << Num_FinishedG + Num_FinishedN + Num_FinishedV << " ";
	OutFile << "[ Norm: " << Num_FinishedN << ", Veg: " << Num_FinishedG << ", VIP: " << Num_FinishedV << " ]" << endl;
	OutFile << "Cooks: " << Num_NormalCook + Num_VIPCook + Num_VeganCook << " ";
	OutFile << "[ Norm: " << Num_NormalCook << ", Veg: " << Num_VeganCook << ", VIP: " << Num_VIPCook << ", Injured: " << Num_InjuredCooks << " ]" << endl; 
	double Avg_Wait = ( sum_1 * 1.0 ) / (Num_FinishedG + Num_FinishedN + Num_FinishedV);
	double Avg_Serv = ( sum_2 * 1.0 ) / (Num_FinishedG + Num_FinishedN + Num_FinishedV);
	OutFile << "Avg Wait = " << Avg_Wait << ", Avg Serv = " << Avg_Serv << endl;
	OutFile << "Urgent Orders:" << Num_UrgentOrders << "," << "  Auto-promoted: " << Auto_promoted*100.0/(Num_FinishedN + Auto_promoted)  << "%" << endl;
	OutFile.close();
}
/// ==================================================================================================
Order** Restaurant::Sorted_FinishedOrders(Queue<Order*> pO, int& count)
{
	count = 0;
	Order** pOR = Finished_Orders.toArray(count);
	for ( int i =0 ; i < count -1 ; i++)
	{
		if ( pOR[i]->get_FinishTime() == pOR[i+1] ->get_FinishTime() )
		{
			if( pOR[i]->get_FinishTime() - pOR[i]->getServeTime() > pOR[i+1]->get_FinishTime() - pOR[i+1]->getServeTime() )
			{
				Order* temp = pOR[i];
				pOR[i] = pOR[i+1];
				pOR[i+1] = temp;
			}
		}
	}
	return pOR;
}
/// ==================================================================================================
/// ==================================================================================================

/// ================================================================================================== 
///					          Printing Information and Drawing Data Handling Functions 		
/// ==================================================================================================
void Restaurant::FillDrawingList()
{
	Avail_CookN = 0;
	Avail_CookG = 0;
	Avail_CookV = 0;
	int Count;
	Cook** pC = Sort_Cook(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if ( pC[i] ->get_Assigned() == Not_Assigned)
		{
				pGUI ->AddToDrawingList(pC[i]);
				if( pC[i] ->GetType() == TYPE_NRM )
						Avail_CookN++;
				else if( pC[i] ->GetType() == TYPE_VIP)
						Avail_CookV++;
				else
					Avail_CookG++;
		}
	}
	Order** pO = VIP_Orders.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
			pGUI ->AddToDrawingList(pO[i]);
	}
	pO = Vegan_Orders.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
			pGUI ->AddToDrawingList(pO[i]);
	}
	pO = Normal_Orders.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
			pGUI ->AddToDrawingList(pO[i]);
	}
	pO = InService_Orders.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		pGUI ->AddToDrawingList(pO[i]);
	}
	pO = Finished_Orders.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		pGUI ->AddToDrawingList(pO[i]);
	}	
}
/// ==================================================================================================
string Restaurant::Print_Info_1(int TimeStep)
{
	char timestep[10];
	itoa(TimeStep,timestep,10);
	string msg1 = timestep;
	string msg2 = "TS: ";
	return msg2 + msg1;
}
/// ==================================================================================================
string Restaurant::Print_Info_2()
{
	return "Num of waiting orders: Normal(" + to_string(Normal_Orders.Get_Length()) +"), Vegan(" + to_string(Vegan_Orders.Get_Length()) + "), VIP(" 
					+ to_string(VIP_Orders.Get_Length()) +")";
}
/// ==================================================================================================
string Restaurant::Print_Info_3()
{
	return "Num of available Cooks: Normal(" + to_string(Avail_CookN) +"), Vegan(" + to_string(Avail_CookG) + "), VIP(" 
					+ to_string(Avail_CookV) +")";
}
/// ==================================================================================================
string Restaurant::Print_Info_4()
{
	int Count;
	string msg, check;
	Cook** pC =  VIP_Cooks.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		check = pC[i]->get_AssignedOrd_Info();
		if ( check != "No" )
			msg += check + " ";
	}
	pC = Normal_Cooks.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		check = pC[i]->get_AssignedOrd_Info();
		if ( check != "No" )
			msg += check+ " ";
	}
	pC = Vegan_Cooks.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		check = pC[i]->get_AssignedOrd_Info();
		if ( check != "No" )
			msg += check+ " ";
	}
	if (msg.length() <= 1 )
		msg = "NO Assigned Orders";
	return msg;
}
/// ==================================================================================================
string Restaurant::Print_Info_5()
{
	return  "Num of Served Orders: Normal(" + to_string(Num_FinishedN) +"), Vegan(" + to_string(Num_FinishedG) + "), VIP(" 
					+ to_string(Num_FinishedV) +")";
}
/// ==================================================================================================
Cook** Restaurant::Sort_Cook(int & Count)
{
	int Count_N, Count_G, Count_V; 
	Cook** pC_N =  Normal_Cooks.toArray(Count_N);
	Cook** pC_G =  Vegan_Cooks.toArray(Count_G);
	Cook** pC_V =  VIP_Cooks.toArray(Count_V);
	Count = Count_N + Count_G + Count_V;
	Cook** pC = new Cook*[Count];
	for(int i = 0; i < Count_N; i++)
	{
		pC[i] = pC_N[i];
	}
	for(int i = 0; i < Count_G; i++)
	{
		pC[i+Count_N] = pC_G[i];
	}
	for(int i = 0; i < Count_V; i++)
	{
		pC[i+Count_G+Count_N] = pC_V[i];
	}
	for( int i = 0; i < Count ; i++)
	{
		Cook* Max = pC[i];
		int index = i;
		for( int j= i+1; j < Count ; j++)
		{
			if( Max ->Avail_Time() < pC[j] ->Avail_Time())
			{
					Max = pC[j];
					index = j;
			}
		}
		Cook* temp = pC[i];
		pC[i] = pC[index];
		pC[index] = temp;
	}
	return pC;
}
/// ==================================================================================================
void Restaurant::All_AvailCook()
{
	int Count;
	Cook** pC =  VIP_Cooks.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		pC[i] ->Set_Assigned_Status(Not_Assigned);
	}
	pC =  Normal_Cooks.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		pC[i] ->Set_Assigned_Status(Not_Assigned);
	}
	pC =  Vegan_Cooks.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		pC[i] ->Set_Assigned_Status(Not_Assigned);
	}
	pC = InRest_Cook.toArray(Count);
	for(int i = 0; i < Count; i++)
	{
		pC[i] ->Set_Assigned_Status(Not_Assigned);
		InRest_Cook.Remove_Entry(pC[i]);
		if( pC[i]->GetType() == TYPE_NRM)
			{
				Normal_Cooks.Insert(Normal_Cooks.Get_Length(),pC[i]);
				Num_NormalCook++;
			}
			else if ( pC[i]->GetType() == TYPE_VIP)
			{
				VIP_Cooks.Insert(VIP_Cooks.Get_Length(),pC[i]);
				Num_VIPCook++;
			}
			else
			{
				Vegan_Cooks.Insert(Vegan_Cooks.Get_Length(),pC[i]);
				Num_VeganCook++;
			}
	}
}
/// ==================================================================================================
/// ==================================================================================================

/// ================================================================================================== 
///					          Assigned Orders Handling Functions 		
/// ==================================================================================================
void Restaurant::Assign_Orders(int CurrentStep)
{
	Order* pO;
	while ( Get_AvailableCook(VIP_Cooks) != -1 && !VIP_Orders.Is_Empty())
	{
		 pO = VIP_Orders.Get_Entry(0);
		 VIP_Orders.Remove_Entry(pO);
		 pO->setStatus(SRV);
		 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		 int Index_Cook = Get_AvailableCook(VIP_Cooks);
		 VIP_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentStep);
	}
	if (!VIP_Orders.Is_Empty())
	{
			while ( Get_AvailableCook(Normal_Cooks) != -1 && !VIP_Orders.Is_Empty())
			{
				 pO = VIP_Orders.Get_Entry(0);
				 VIP_Orders.Remove_Entry(pO);
				 pO->setStatus(SRV);
				 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
				 int Index_Cook = Get_AvailableCook(Normal_Cooks);
				 Normal_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentStep);
			}
	}
	if (!VIP_Orders.Is_Empty())
	{
			while ( Get_AvailableCook(Vegan_Cooks) != -1 && !VIP_Orders.Is_Empty() )
			{
				 pO = VIP_Orders.Get_Entry(0);
				 VIP_Orders.Remove_Entry(pO);
				 pO->setStatus(SRV);
		         InService_Orders.Insert(InService_Orders.Get_Length(),pO);
				 int Index_Cook = Get_AvailableCook(Vegan_Cooks);
				 Vegan_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentStep);
			}
	}
	while (Get_AvailableCook(Vegan_Cooks) != -1 && Vegan_Orders.peekFront(pO))
	{
		 Vegan_Orders.dequeue(pO);
		 pO->setStatus(SRV);
		 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		 int Index_Cook = Get_AvailableCook(Vegan_Cooks);
		 Vegan_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentStep);
	}
	while (Get_AvailableCook(Normal_Cooks) != -1 && Normal_Orders.Get_Entry(0))
	{
		 pO = Normal_Orders.Get_Entry(0);
		 Normal_Orders.Remove_Entry(pO);
		 pO->setStatus(SRV);
		 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		 int Index_Cook = Get_AvailableCook(Normal_Cooks);
		 Normal_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentStep);
	}

	if(Normal_Orders.Get_Entry(0))
	{
		while ( Get_AvailableCook(VIP_Cooks) != -1 && Normal_Orders.Get_Entry(0) )
		{
		  pO = Normal_Orders.Get_Entry(0);
		 Normal_Orders.Remove_Entry(pO);
		 pO->setStatus(SRV);
		 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		 int Index_Cook = Get_AvailableCook(VIP_Cooks);
		 VIP_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentStep);
		}
	}
}
/// ==================================================================================================
int Restaurant::Get_AvailableCook(List<Cook*> & Co)
{
	int Count;
	Cook** pCo = Co.toArray(Count);
	for ( int i =0; i < Count ; i++)
	{
		if ( pCo[i] ->get_Assigned() == Not_Assigned )
			return i;
	}
	return -1;
}
/// ==================================================================================================
void Restaurant::Advance_Order(int CurrentTimeStep)
{
	Cook* pC = nullptr;
	for ( int i =0 ; i < Num_VIPCook ; i++ )
	{
		if ( VIP_Cooks.Get_Entry(i) ->get_Assigned() == Assigned || VIP_Cooks.Get_Entry(i) ->get_Assigned() == Injured || VIP_Cooks.Get_Entry(i) ->get_Assigned() == Assigned_Urgently )
		{
			Order* pOrd = VIP_Cooks.Get_Entry(i) ->Advance_AssignedOrd(CurrentTimeStep);	
			if (pOrd)
			{
				InService_Orders.Remove_Entry(pOrd);
				Finished_Orders.enqueue(pOrd);
				if (pOrd->GetType() == TYPE_NRM ) Num_FinishedN++;
				else if (pOrd->GetType() == TYPE_VIP ) Num_FinishedV++;
				else Num_FinishedG++;
				if ( VIP_Cooks.Get_Entry(i) ->get_Assigned() == In_Rest )
				{
					pC = VIP_Cooks.Get_Entry(i);
				    VIP_Cooks.Remove_Entry(pC);
					InRest_Cook.Insert(InRest_Cook.Get_Length(),pC);
					Num_VIPCook--;
					i--;
				}
			}
		}
	}
	for ( int i =0 ; i < Num_NormalCook ; i++ )
	{
		if ( Normal_Cooks.Get_Entry(i) ->get_Assigned() == Assigned || Normal_Cooks.Get_Entry(i) ->get_Assigned() == Injured || Normal_Cooks.Get_Entry(i) ->get_Assigned() == Assigned_Urgently )
		{
			Order* pOrd = Normal_Cooks.Get_Entry(i) ->Advance_AssignedOrd(CurrentTimeStep);	
			if (pOrd)
			{
				InService_Orders.Remove_Entry(pOrd);
				Finished_Orders.enqueue(pOrd);
				if (pOrd->GetType() == TYPE_NRM ) Num_FinishedN++;
				else if (pOrd->GetType() == TYPE_VIP ) Num_FinishedV++;
				else Num_FinishedG++;
				if ( Normal_Cooks.Get_Entry(i) ->get_Assigned() == In_Rest )
				{
					pC = Normal_Cooks.Get_Entry(i);
				    Normal_Cooks.Remove_Entry(pC);
					InRest_Cook.Insert(InRest_Cook.Get_Length(),pC);
					Num_NormalCook--;
					i--;
				}
			}
		}
	}
	for ( int i =0 ; i < Num_VeganCook ; i++ )
	{
		if ( Vegan_Cooks.Get_Entry(i) ->get_Assigned() == Assigned || Vegan_Cooks.Get_Entry(i) ->get_Assigned() == Injured || Vegan_Cooks.Get_Entry(i) ->get_Assigned() == Assigned_Urgently  )
		{
			Order* pOrd = Vegan_Cooks.Get_Entry(i) ->Advance_AssignedOrd(CurrentTimeStep);	
			if (pOrd)
			{
				InService_Orders.Remove_Entry(pOrd);
				Finished_Orders.enqueue(pOrd);
				if (pOrd->GetType() == TYPE_NRM ) Num_FinishedN++;
				else if (pOrd->GetType() == TYPE_VIP ) Num_FinishedV++;
				else Num_FinishedG++;
				if ( Vegan_Cooks.Get_Entry(i) ->get_Assigned() == In_Rest )
				{
					pC = Vegan_Cooks.Get_Entry(i);
				    Vegan_Cooks.Remove_Entry(pC);
					InRest_Cook.Insert(InRest_Cook.Get_Length(),pC);
					Num_VeganCook--;
					i--;
				}
			}
		}
	}
}
/// ==================================================================================================
void Restaurant::Auto_PromoteNormal(int CurrentTimeStep)
{
	int Count = 0;
	Order** pO = Normal_Orders.toArray(Count);
	for ( int i = 0 ; i < Count ; i ++)
	{
		if ( (CurrentTimeStep - pO[i] ->get_ArrTime()) > AutoPromotion)
		{
			Event* pE = new Promotion_Event(pO[i] ->get_ArrTime(),pO[i]->GetID(),0.0);
			pE->Execute(this);
			Auto_promoted ++;
		}
	}
}
/// ==================================================================================================
/// ==================================================================================================

/// ================================================================================================== 
///					          Urgent Orders Handling Functions 		
/// ==================================================================================================
void Restaurant::Urgent_Orders(int CurrentTimeStep)
{
	Order* pO = nullptr;
	Insert_UrgentOrders(CurrentTimeStep);
	while ( Get_AvailableCook(VIP_Cooks) != -1 && !Urgent_Order.Is_Empty())
	{
		 pO = Urgent_Order.Get_Entry(0);
		 Urgent_Order.Remove_Entry(pO);
		 pO->setStatus(SRV);
		 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		 int Index_Cook = Get_AvailableCook(VIP_Cooks);
		 VIP_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentTimeStep);
	}
	if (!Urgent_Order.Is_Empty())
	{
			while ( Get_AvailableCook(Normal_Cooks) != -1 && !Urgent_Order.Is_Empty())
			{
				 pO = Urgent_Order.Get_Entry(0);
				 Urgent_Order.Remove_Entry(pO);
				 pO->setStatus(SRV);
				 InService_Orders.Insert(InService_Orders.Get_Length(),pO);
				 int Index_Cook = Get_AvailableCook(Normal_Cooks);
				 Normal_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentTimeStep);
			}
	}
	if (!Urgent_Order.Is_Empty())
	{
			while ( Get_AvailableCook(Vegan_Cooks) != -1 && !Urgent_Order.Is_Empty() )
			{
				 pO = Urgent_Order.Get_Entry(0);
				 Urgent_Order.Remove_Entry(pO);
				 pO->setStatus(SRV);
		         InService_Orders.Insert(InService_Orders.Get_Length(),pO);
				 int Index_Cook = Get_AvailableCook(Vegan_Cooks);
				 Vegan_Cooks.Get_Entry(Index_Cook) ->Assign_Order(pO,CurrentTimeStep);
			}
	}
	while(Get_InBreakCook(VIP_Cooks) != -1 && !Urgent_Order.Is_Empty())
	{
		pO = Urgent_Order.Get_Entry(0);
		Urgent_Order.Remove_Entry(pO);
		pO ->setStatus(SRV);
		InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		int Index_Cook = Get_InBreakCook(VIP_Cooks);
		VIP_Cooks.Get_Entry(Index_Cook)->Assign_UrgentOrder(pO,CurrentTimeStep);
	}
	while(Get_InBreakCook(Normal_Cooks) != -1 && !Urgent_Order.Is_Empty())
	{
		pO = Urgent_Order.Get_Entry(0);
		Urgent_Order.Remove_Entry(pO);
		pO ->setStatus(SRV);
		InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		int Index_Cook = Get_InBreakCook(Normal_Cooks);
		Normal_Cooks.Get_Entry(Index_Cook)->Assign_UrgentOrder(pO,CurrentTimeStep);
	}
	while(Get_InBreakCook(Vegan_Cooks) != -1 && !Urgent_Order.Is_Empty())
	{
		pO = Urgent_Order.Get_Entry(0);
		Urgent_Order.Remove_Entry(pO);
		pO ->setStatus(SRV);
		InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		int Index_Cook = Get_InBreakCook(Vegan_Cooks);
		Vegan_Cooks.Get_Entry(Index_Cook)->Assign_UrgentOrder(pO,CurrentTimeStep);
	}
	while (!InRest_Cook.Is_Empty() && !Urgent_Order.Is_Empty())
	{
		pO = Urgent_Order.Get_Entry(0);
		Urgent_Order.Remove_Entry(pO);
		pO ->setStatus(SRV);
		InService_Orders.Insert(InService_Orders.Get_Length(),pO);
		Cook* pC = InRest_Cook.Get_Entry(0);
		InRest_Cook.Remove_Entry(pC);
		pC->Assign_UrgentOrder(pO,CurrentTimeStep);
		if (pC->GetType() == TYPE_NRM)
		{
			Normal_Cooks.Insert(Normal_Cooks.Get_Length(),pC);
			Num_NormalCook++;
		}
		else if ( pC->GetType() == TYPE_VIP)
		{
			VIP_Cooks.Insert(VIP_Cooks.Get_Length(),pC);
			Num_VIPCook++;
		}
		else
		{
			Vegan_Cooks.Insert(Vegan_Cooks.Get_Length(),pC);
			Num_VeganCook++;
		}
	}
}
/// ================================================================================================== 
void Restaurant::Insert_UrgentOrders(int CurrentTimeStep)
{
	int count = 0;
	Order** pO = VIP_Orders.toArray(count);
	for ( int i =0; i < count ; i++)
	{
		if( CurrentTimeStep - pO[i]->get_ArrTime() > VIP_WT)
		{
			Num_UrgentOrders++;
			VIP_Orders.Remove_Entry(pO[i]);
		    Urgent_Order.Insert(Urgent_Order.Get_Length(),pO[i]);
		}
	}
}
/// ================================================================================================== 
/// ================================================================================================== 

/// ================================================================================================== 
///					          Cooks Handling Functions 		
/// ==================================================================================================
int Restaurant::Get_FirstBusyCook(List<Cook*> & Co)
{
	int Count;
	Cook** pCo = Co.toArray(Count);
	for ( int i =0; i < Count ; i++)
	{
		if ( pCo[i] ->get_Assigned() == Assigned)
			return i;
	}
	return -1;
}
/// ================================================================================================== 
int Restaurant::Get_InBreakCook(List<Cook*> & Co)
{
	int Count;
	Cook** pCo = Co.toArray(Count);
	for ( int i =0; i < Count ; i++)
	{
		if ( pCo[i] ->get_Assigned() == In_Break )
			return i;
	}
	return -1;
}
/// ================================================================================================== 
void Restaurant::Break_Time()
{
	int Count;
	Cook** pO = Normal_Cooks.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if( pO[i] ->Get_Num_OrdDone() == pO[i]->Get_NumOrder() )
			pO[i] ->Set_Assigned_Status(In_Break);
	}
	pO = VIP_Cooks.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if( pO[i] ->Get_Num_OrdDone() == pO[i]->Get_NumOrder() )
			pO[i] ->Set_Assigned_Status(In_Break);
	}
	pO = Vegan_Cooks.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if( pO[i] ->Get_Num_OrdDone() == pO[i]->Get_NumOrder() )
			pO[i] ->Set_Assigned_Status(In_Break);
	}
}
/// ================================================================================================== 
void Restaurant::Return_Break()
{
	int Count;
	Cook** pO = Normal_Cooks.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if (pO[i] ->Get_Break_StartTime() == pO[i]->GetBreak()&&  pO[i] ->get_Assigned() == In_Break)
			pO[i] ->Set_Assigned_Status(Not_Assigned);
		else if (pO[i] ->get_Assigned() == In_Break)
				pO[i] ->Advance_BreakDuration();
	}
	pO = VIP_Cooks.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if (pO[i] ->Get_Break_StartTime() == pO[i]->GetBreak() && pO[i] ->get_Assigned() == In_Break )
			pO[i] ->Set_Assigned_Status(Not_Assigned);
		else if (pO[i] ->get_Assigned() == In_Break)
			pO[i] ->Advance_BreakDuration();
	}
	pO = Vegan_Cooks.toArray(Count);
	for ( int i = 0; i < Count ; i++)
	{
		if (pO[i] ->Get_Break_StartTime() == pO[i]->GetBreak() && pO[i] ->get_Assigned() == In_Break )
			pO[i] ->Set_Assigned_Status(Not_Assigned);
		else if (pO[i] ->get_Assigned() == In_Break)
			pO[i] ->Advance_BreakDuration();
	}
}
/// ================================================================================================== 
void Restaurant::Return_Rest()
{
	int count =0;
	Cook** pC = InRest_Cook.toArray(count);
	for ( int i = 0; i < count ; i++)
	{
		if (pC[i]->get_RstPrd() == pC[i]->Get_RstPrd_Duration())
		{
			pC[i] ->Set_Assigned_Status(Not_Assigned);
			InRest_Cook.Remove_Entry(pC[i]);
			if( pC[i]->GetType() == TYPE_NRM)
			{
				Normal_Cooks.Insert(Normal_Cooks.Get_Length(),pC[i]);
				Num_NormalCook++;
			}
			else if ( pC[i]->GetType() == TYPE_VIP)
			{
				VIP_Cooks.Insert(VIP_Cooks.Get_Length(),pC[i]);
				Num_VIPCook++;
			}
			else
			{
				Vegan_Cooks.Insert(Vegan_Cooks.Get_Length(),pC[i]);
				Num_VeganCook++;
			}
		}
		else
			pC[i] ->Advance_RstPrd_Duration();
	}
}
/// ================================================================================================== 
void Restaurant::Injured_BusyCook()
{
	double R = ((rand() % 100) + 1)/100.0;
	int Index_Cook = -1;
	if ( R <= InjProp )
	{
		if(Get_FirstBusyCook(VIP_Cooks) != -1)
		{
			Index_Cook = Get_FirstBusyCook(VIP_Cooks);
			VIP_Cooks.Get_Entry(Index_Cook)->Set_Assigned_Status(Injured);
			Num_InjuredCooks++;
			return;
		}
		else if (Get_FirstBusyCook(Normal_Cooks) != -1)
		{
			Index_Cook = Get_FirstBusyCook(Normal_Cooks);
			Normal_Cooks.Get_Entry(Index_Cook)->Set_Assigned_Status(Injured);
			Num_InjuredCooks++;
			return;
		}
		else if (Get_FirstBusyCook(Vegan_Cooks) != -1)
		{
			Index_Cook = Get_FirstBusyCook(Vegan_Cooks);
			Vegan_Cooks.Get_Entry(Index_Cook)->Set_Assigned_Status(Injured);
			Num_InjuredCooks++;
			return;
		}
	}
}
/// ================================================================================================== 
/// ================================================================================================== 