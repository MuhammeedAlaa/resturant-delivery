#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
#include "Restaurant.h"
#include "..\Events\ArrivalEvent.h"
#include "..\Events\CancelationEvent.h"
#include "..\Events\PromotionEvent.h"
using namespace std;


Restaurant::Restaurant(GUI* p) 
{
	pGUI = p;
	for (int i = 0; i < REG_CNT; i++)
	{
		 NServedOrd[i] = 0;
		 FServedOrd[i] = 0;
		 VServedOrd[i] = 0;
	}
}



void Restaurant::RunSimulation()
{
	pGUI = new GUI;
	PROG_MODE	mode = pGUI->getGUIMode();
	switch (mode)	//Add a function for each mode in next phases
	{
		case MODE_INTR:
			Simulate(mode);
			break;
		case MODE_STEP:
			Simulate(mode);
			break;
		case MODE_SLNT:
			Silent();
			break;
		case MODE_EXIT :
			break;
	};

}

bool Restaurant::wait(PROG_MODE mode)
{
	if(mode == MODE_INTR)		   // Waiting to show changes before assigning motorcycles to orders
	{
		if(pGUI->waitForClick(this))
			return true;
		return false;
	}
	else if(mode == MODE_STEP)
	{
		for (int i = 0; i < 10; i++)
		{
			if(pGUI->waitForClick(this, duration_cast< milliseconds >(system_clock::now().time_since_epoch())))
				return true;
			else
				Sleep(45);
		}
		return false;
	}
		
}

////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//								different setters and getters								  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

int Restaurant::GetAutoProm()const{ return AutoProm; }

void Restaurant::SetAutoProm(int x){ AutoProm = x > 0 ? x : 10; }

Region* Restaurant::GetRegion(int index){ return &R[index]; }

void Restaurant::SetTimeDam(int t){ TimeDam = t ;}

void Restaurant::SetTimeTir(int t ){  TimeTir = t;}

GUI* Restaurant::GetGUI()const{return pGUI;}

////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//									Event handling functions								  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

//adds a new event to the queue of events
void Restaurant::AddEvent(Event* pE)	
{
	EventsQueue.enqueue(pE);
}

//Executes ALL events that should take place at current timestep
void Restaurant::ExecuteEvents(int CurrentTimeStep)  
{
	Event *pE;
	while( EventsQueue.peekFront(pE) )	//as long as there are more events
	{
		if(pE->getEventTime() > CurrentTimeStep )	//no more events at current time
			return;

		pE->Execute(this);
		EventsQueue.dequeue(pE);	//remove event from the queue
		delete pE;		//deallocate event object from memory
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//									input reading function									  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////


bool Restaurant::ReadFile()
{
	InputFile loadfile(this, pGUI);
	if(loadfile.Read())
		return true;
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//								Different modes functions									  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

void Restaurant::Simulate(PROG_MODE mode)    // Interactive mode
{
	//prompt the user again and again till a valid file name is entered to read inputs
	while(!ReadFile()) {}	
	OutputFile Out(this, pGUI);
	int currstep = 1;
	char timestep[10];

	// to store the orders that were assigned as well as their motors at this timestep
	string  PrintAssigned;

	//print the different info about the different regions
	PrintStatusBar(PrintAssigned);

	//flag to check if there is still an order not served so simulation doesn't stop
	bool FlagOrd=true,FlagunAssign[4]={true,true,true,true};
	
	while(!EventsQueue.isEmpty() || FlagOrd || FlagunAssign[0] || FlagunAssign[1] || FlagunAssign[2] || FlagunAssign[3])
	{
		
		ExecuteEvents(currstep);
		_itoa_s(currstep,timestep,10);

		if(wait(mode))
			return;						                // Waiting according to the mode of the simulation
		RestUpdate(timestep, PrintAssigned);		    // Updates the interface 
		if(wait(mode))
			return;
		AssignOrders(currstep, PrintAssigned);		   //Assign the order whose time has come
		FlagOrd= RestUpdate(timestep, PrintAssigned);  //Updates the interface and sets the order flag
		PrintAssigned = "";

		//to show that there is an order that reached its finish time so we need to write it on the output file
		bool FlagPrint = true;

		while(ServedOrder.getcount() && FlagPrint)
		{
			Order* tmpord = ServedOrder.getmax();

			if(tmpord->GetFinishTime() == currstep)
			{
				if(tmpord->GetType() == TYPE_NRM)
					//to count served normal orders so far
					NServedOrd[tmpord->GetRegion()]++;

				if(tmpord->GetType() == TYPE_VIP)
					//to count served VIP orders so far
					VServedOrd[tmpord->GetRegion()]++;

				if(tmpord->GetType() == TYPE_FROZ)
					//to count served frozen orders so far
					FServedOrd[tmpord->GetRegion()]++;

				// print the order then delete it 
				Out.Write(tmpord);
				delete tmpord;
				ServedOrder.extractMax();
				FlagPrint = true;
			}
			else
				FlagPrint = false;
		}

		for (int i = 0; i < 4; i++)
		{
			FlagunAssign[i] = R[i].UnAssignMotors(currstep);
		}
		currstep++;
		for (int i = 0; i < REG_CNT; i++)
		{
			R[i].Promote(AutoProm, currstep);
		}
	}
		PrintAssigned = "";  
	
	//print the statistics for each region(# of orders, # of motors, Avg wait time, ..etc)
	Out.PrintStatstics();	

	pGUI->PrintMessage("Simulation done, click to END program");
	pGUI->waitForClick(this);

}


void Restaurant::Silent()
{
	//prompt the user again and again till a valid file name is entered to read inputs
	while(!ReadFile()) {}			 
	OutputFile Out(this, pGUI);
	int currstep = 1;

	//flag to check if there is still an order not served so simulation doesn't stop
	bool FlagOrd=true,FlagunAssign[4]={true,true,true,true};
	
	while(!EventsQueue.isEmpty() || FlagOrd || FlagunAssign[0] || FlagunAssign[1] || FlagunAssign[2] || FlagunAssign[3])
	{
		ExecuteEvents(currstep);
		
		//if there is still orders not served simulation doesn't stop
		for (int i = 0; i < REG_CNT; i++)
		{
			if(R[i].GetVOrdCnt() || R[i].GetFOrdCnt()|| R[i].GetNOrdCnt())
			{
				FlagOrd = true;
				break;
			}
			else
				FlagOrd = false;
		}
		
		//Assign the order whose time has come
		string tmp;
		AssignOrders(currstep, tmp);

		//update the interface after deleting the orders whose time has come
		//FlagOrd=LoadGUI();
		for (int i = 0; i < 4; i++)
		{
			FlagunAssign[i] = R[i].UnAssignMotors(currstep);
		}
		bool FlagPrint = true;

		//to show that there is an order that whose finish time has come so we need to write it on the output file
		while(ServedOrder.getcount() && FlagPrint)
		{
			Order* tmpord = ServedOrder.getmax();
			if(tmpord->GetFinishTime() == currstep)
			{

					Out.Write(tmpord);
					delete tmpord;
					ServedOrder.extractMax();
					FlagPrint = true;
			}
			else
				FlagPrint = false;
		}
		currstep++;
		for (int i = 0; i < REG_CNT; i++)
		{
			R[i].Promote(AutoProm, currstep);
		}
	}
		Out.PrintStatstics();	
		pGUI->PrintMessage("Simulation done, click any where to close the program.");
		pGUI->waitForClick(this);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//				Preparing strings to be printed on the status bar							  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

void Restaurant::PrintStatusBar(string actions)
{
	// used to store the orders in service of the previous timestep with its the motorcylce 
	string assigned[REG_CNT];
	if(actions.length() > 2)
	{
		int pos[4];
		pos[0] = actions.find("Region1") + 7;
		pos[1] = actions.find("Region2") + 7;
		pos[2] = actions.find("Region3") + 7;
		pos[3] = actions.find("Region4") + 7;
		for (int i = 0; i < 3; i++)
		{
			assigned[i] += " " + actions.substr(pos[i], pos[i + 1] - pos[i] - 7);
		}
		assigned[3] += actions.substr(pos[3]);
	}


	//to store the number of active orders of each type
	string RN[REG_CNT], RV [REG_CNT], RF[REG_CNT], RALL[REG_CNT];
	for (int i = 0; i < REG_CNT; i++)
	{
		RN[i] = "NMotors: "+to_string(R[i].Get_NMotorCnt())+",     NOrders: "+to_string(R[i].GetNOrdCnt());
		RV[i] = ",     VMotors: "+to_string(R[i].Get_VMotorCnt())+",     VOrders: "+to_string(R[i].GetVOrdCnt());
		RF[i] = ",     FMotors: "+to_string(R[i].Get_FMotorCnt())+",     FOrders: "+to_string(R[i].GetFOrdCnt());
		RALL[i] = "[Nord:"+to_string(NServedOrd[i])+ ", Ford :" +to_string(FServedOrd[i]) +", Vord:" +to_string(VServedOrd[i])+"]";
	}			
	string msg0 = "RegionA   " + RN[0] + RV[0] + RF[0] + assigned[0];
	string msg1 = "RegionB   " + RN[1] + RV[1] + RF[1] + assigned[1];
	string msg2 = "RegionC   " + RN[2] + RV[2] + RF[2] + assigned[2];
	string msg3 = "RegionD   " + RN[3] + RV[3] + RF[3] + " " + assigned[3];


	// to store the number of served orders for each region
	string msg4 = "Served orders so far: RegionA " + RALL[0] + ",  RegionB " + RALL[1] +",  RegionC " + RALL[2] +",  RegionD " + RALL[3];
	pGUI->PrintMessage(msg0, msg1, msg2, msg3, msg4);

}




////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//				responsible for showing the changes on the GUI each timestep				  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

bool Restaurant::RestUpdate(string timestep, string actions)
{
		bool check = LoadGUI();
		pGUI->UpdateInterface();
		pGUI->DrawTimeStepCenter(timestep);
		pGUI->CreateDrawToolBar();
		PrintStatusBar(actions);
		return check;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//				loads the GUI with the active orders to be drawn							  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

bool Restaurant::LoadGUI()
{
	Order*const* tmpArr;

	//flag if true means there is still orders waiting to be seved
	bool flag = false;
	Order*tmpOrd;
	for (int j = 0; j < REG_CNT; j++)
	{
		//load the gui with the VIP orders
		int OrderCnt = R[j].GetVOrdCnt();
		
		// to store the vip orders then put them again in the priorityQ after filling the gui array
		Queue<Order*> tmpQ;
		for (int i = 0; i < OrderCnt; i++)
		{
			tmpOrd = R[j].dequeueV();
			pGUI->AddOrderForDrawing(tmpOrd);
			tmpQ.enqueue(tmpOrd);
			flag=true;
		}
		while(!tmpQ.isEmpty())
		{
			tmpQ.dequeue(tmpOrd);
			R[j].InsertVOrder(tmpOrd);
		}


		//load the gui with the frozen orders
		tmpArr = R[j].GetArrFOrd();
		OrderCnt = R[j].GetFOrdCnt();
		for (int i = 0; i < OrderCnt; i++)
		{
			pGUI->AddOrderForDrawing(tmpArr[i]);
			flag=true;
		}
		delete []tmpArr;
		tmpArr = nullptr;


		//load the gui with normal orders
		tmpArr = R[j].GetArrNOrd();
		OrderCnt = R[j].GetNOrdCnt();
		for (int i = 0; i < OrderCnt; i++)
		{
			pGUI->AddOrderForDrawing(tmpArr[i]);
			flag=true;
		}
		delete tmpArr;
		tmpArr = nullptr;
	}

	return flag;

}

////////////////////////////////////////////////////////////////////////////////////////////////
//																							  //
//				to assign motorcycles for the orders for all regions						  //
//																							  //	
////////////////////////////////////////////////////////////////////////////////////////////////

void Restaurant::AssignOrders(int timestep, string& s)
{
	for (int i = 0; i < 4; i++)
	{
		// the string s is used to keep track of the assigned orders each timestep to be drawn on the status bar
		s += "Region";
		s += to_string(i + 1);

		//	Returns the recovered motorcycles to the idle lists before assiging
		R[i].recovered(timestep);

		// start with assigning vip order in each region if exists to a motorcycle
		if(R[i].GetVOrdCnt())
			R[i].AssignOrdVMotor(timestep,TimeDam,TimeTir, &ServedOrder, s);

		//then frozen order in each region in case exists to a frozen motorcycle 
		 if(!R[i].FOrdisEmpty())
			R[i].AssignOrdFMotor(timestep,TimeDam,TimeTir, &ServedOrder, s);

		//Assign first waiting Normal order in each region case exists to a Motorcycle 
		if(R[i].GetNOrdCnt())
			R[i].AssignOrdNMotor(timestep,TimeDam,TimeTir, &ServedOrder, s);
		s += " ";
	}

}

Restaurant::~Restaurant()
{
	if(pGUI)
		delete pGUI;
}

