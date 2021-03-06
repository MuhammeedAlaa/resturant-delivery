#include "ArrivalEvent.h"
#include "..\Rest\Restaurant.h"


ArrivalEvent::ArrivalEvent(int eTime, int oID, ORD_TYPE oType, REGION reg):Event(eTime, oID)
{
	OrdType = oType;
	OrdRegion = reg;
}

ArrivalEvent::ArrivalEvent(int eTime, int oID, ORD_TYPE oType, REGION reg, double dist, double mon,bool h):Event(eTime, oID)
{
	OrdType = oType;
	OrdRegion = reg;
	OrdMoney = mon > 0 ? mon :0;
	OrdDistance = dist> 0 ? dist :0;
	hard = h;
}




void ArrivalEvent::Execute(Restaurant* pRest)
{
	//This function should create an order and fills its data 
	// Then adds it to normal, frozen, or VIP order lists that you will create in phase1
	
	Order* pOrd = new Order(EventTime ,OrderID, OrdType, OrdRegion, OrdDistance, OrdMoney,hard);
	//filling the order to the lists in its region
	if(OrdType == TYPE_NRM)
		pRest->GetRegion(OrdRegion)->InsertNOrder(pOrd);
	else if(OrdType == TYPE_FROZ)
		pRest->GetRegion(OrdRegion)->InsertFOrder(pOrd);
	else if(OrdType == TYPE_VIP)
		pRest->GetRegion(OrdRegion)->InsertVOrder(pOrd);
	else if(OrdType == TYPE_PARTY)
		pRest->GetRegion(OrdRegion)->InsertPOrder(pOrd);
	else
		pRest->GetRegion(OrdRegion)->InsertNearOrder(pOrd);

}
