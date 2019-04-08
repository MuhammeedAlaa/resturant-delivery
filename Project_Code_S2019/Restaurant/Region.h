#pragma once
#include"Generic_DS\PriorityQueue.h"
#include"Rest\Motorcycle.h"
#include"Rest\Order.h"
#include"Generic_DS\Queue.h"
<<<<<<< HEAD
#include"Generic_DS\List.h"
=======
#include"Generic_DS\list.h"
>>>>>>> daa869ce7a94a8505d762bd6fb5fcd17f2be1586
class Region
{
protected:
	//List of Motocylyes REgion
	priorityQueue<Motorcycle*>idelFMotorQ;
	priorityQueue<Motorcycle*>idelNMotorQ;
	priorityQueue<Motorcycle*>idelVMotorQ;
	priorityQueue<Motorcycle*>servFMotorQ;
	priorityQueue<Motorcycle*>serNMotorQ;
	priorityQueue<Motorcycle*>servVMotorQ;

	//Number of MotorCycle in Region
	int N_Motors,V_Motors,F_Motors;

	//Order in Region
	LinkedList<Order*>NOrderQueue;
	priorityQueue<Order*>VOrderQueue;
	Queue<Order*>FOrderQueue;
public:
	Region(void);
	//Function to Fill the Number of MotorCycles and the Lists of the MotorCycles by taking the Number and take 
	//an array of Pointers to each Type (Pirority Doesn't matter in this array because the Fill will handle it) 
	void FillMotors(int ,int , int, Motorcycle**, Motorcycle**, Motorcycle** );
	void InsertFOrder(Order*); //a Functions to insert Frozen orders
	void InsertVOrder(Order*); // a funtion to inset VIP orders 
	void InsertNOrder(Order*);  // a function to insert normal orders 
	int Get_NMotorC()const;
	int Get_VMotorC()const;
	int Get_FMotorC()const;

	// Getters for the counters
	int getVcount();
	int getNcount();
	int getFcount();
	bool Fisempty();

	// Functions used to dequeue the orders
	Order* dequeueN(int );
	Order* dequeueV();
	Order* dequeueF();
	void DeleteMotors();
	int Sreach(Order* );


	~Region(void);
};

