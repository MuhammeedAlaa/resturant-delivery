#ifndef __ORDER_H_
#define __ORDER_H_

#include "..\Defs.h"
#include "Motorcycle.h"
#include"delivery.h"
class Motorcycle;
class delivery;
class Order
{

private:
	int ID;                     //Each order has a unique ID (from 1 --> 999 )
	ORD_TYPE type;	            //order type: Normal, Frozen, VIP
	REGION Region;              //Region of this order
	double Distance;	        //The distance (in meters) between the order location and the resturant 
	double totalMoney;	        //Total order money
	int ArrTime, ServTime, FinishTime, WaitingTime;	    //arrival, service start, finish, and waiting times
	double priority;
	Motorcycle* AssignedMotor; //to calculate the serving and finish time
	delivery* AssignedDelivery;
	void Changepriority(int timestp);
	bool hard;		// boolean to check if the order is hard to deliever due to traffic problems
public:
	Order(int Arrivaltime ,int ID, ORD_TYPE r_Type, REGION r_region, double dist, double mon,bool Hard=false);
	Order(int);
	virtual ~Order();

	//setters and getters
	
	int GetID() const;
	REGION GetRegion() const;
	ORD_TYPE GetType() const;
	double GetDistance() const;
	double GetMoney() const;
	int GetArrTime() const;
	int GetServTime() const;
	int GetFinishTime() const;
	int GetWaitingTime() const;
	double Getpriority() const;
	Motorcycle* GetMotor() const;
	bool ishard();

	void SetID(int);
	void SetRegion(REGION);
	void SetType(ORD_TYPE);
	void SetDistance(double);
	void SetMoney(double);
	void SetArrTime(int);
	void SetServTime(int);
	void SetFinishTime(int);
	void SetWaitingTime(int);
	void sethard(bool);

	//overloads for checking the priority

	bool operator <(Order &v);
	bool operator >(Order& v);
	bool operator ==(Order& v);
	
	void AssignMotor(Motorcycle*, int , int =-1, int =-1);
	void Delivery(delivery* , int , int=-1 , int=-1 );

};

#endif