#include "acequia_manager.h"
#include <iostream>
#include <cmath>
#include <vector>


/*Instructions for this problem:

	The intend of this project is to simulate water management conservation principles in the context of New Mexico

	In this simulation, there exists several Regions (North, South, etc.). Each region class includes both:
	- a given water level
	- a given water need
	- a indicator boolean for if the region is flooded
	- an indicator boolean for if the region is in drought

	With each region, there are given waterSources provided (perhaps a .dat file list each waterSource to  a region) 
	and certain waterSources have canals connected to them to deliver water across regions.

	Given the current state of the region, students wil be asked to utlize the canals that connect regions to
	develop the logic and algorithm for finding a solution. The simulation has a fixed time



	The student solution will be evaluated on the criteria that each region meets the following:
	- a given region is NOT flooded
	- a given region is NOT in drought
	- the region waterNeed does not exceed the region waterLevel 
*/

/*This will be how the solveProblems function is set up. The student may enter their on  */
/*
void solveProblems(AcequiaManager& manager)
{
	//the student can call the members of the canals object such as name of canal. sourceRegion, and destinationRegion
	//This could be helpful in informing the students strategy to solve the problem
	auto canals = manager.getCanals();
	//students may call to get Region and WaterSource objects to inform decisions 


	while(!manager.isSolved && manager.hour!=manager.SimulationMax)
	{	
		//enter student code here


		manager.nexthour();
	}
}
*/


/*Generally, gain can help determine whether a region is in drought or not. 

*/
double gain(Region *r)
{
	double gain = r->waterNeed - r->waterLevel;
	return gain;
}

//THOUSANDS of gallons of water are being moved around. Therefore, we need to develop an algorithm that will convert the water difference to corresponding flow rate.
//do I want rate to change each time? 


void solveProblems(AcequiaManager& manager)
{	
	auto canals = manager.getCanals();
	auto region = manager.getRegions();

	//Assigning regions 
	auto North = region[0];
	auto NorthNeed = region[0]->waterNeed;

	auto South = region[1];
	auto SouthNeed = region[1]->waterNeed;

	auto East = region[2];
	auto EastNeed = region[2]->waterNeed;

	//Assigning canal names based on initialization information from AcequiaManager.cpp
	auto canal_A = canals[0]; //Connecting North -> South
	auto canal_B = canals[1]; //Connecting South -> East
	auto canal_C = canals[2]; //Connecting North -> East
	auto canal_D = canals[3]; //Connecting East -> North

	double rate = 0.5; // let's start with a fixed rate. something moderate.

	while(!manager.isSolved && manager.hour != manager.SimulationMax)
	{

		// Updating current water level through each iteration 
		auto NorthCurrent = North->waterLevel;
		auto SouthCurrent = South->waterLevel;
		auto EastCurrent = East->waterLevel;

		auto NorthGain = gain(North);
		auto SouthGain = gain(South);
		auto EastGain = gain(East);

		/*
			gain[0] = North
			gain[1] = South 
			gain[2] = East
		*/
		// WHAT ACEQUIAMANAGER PROVIDES US:
		// North region is the key component for this system: it has canals connecting water to both South and East.
		// Need to make sure North has enough water to distribute. 
		// Canals are flowing in ONE DIRECTION. 
		// First priority is the North region.


		//------NORTH------- (Assuming North has the most water)
		if(NorthGain > 0) // if north needs water
		{
			//open canal D 
			canal_D -> toggleOpen(true);
			canal_D->setFlowRate(rate);
		} else 
		{
			canal_D->toggleOpen(false);
		}

		//-----SOUTH-------
		if(SouthGain > 0) //if south needs water
		{
			//open canal A		
			canal_A->toggleOpen(true);
			canal_A->setFlowRate(rate);	
		} else
		{
			canal_A->toggleOpen(false);
		}

		//-------EAST-------
		if(EastGain > 0)// if East needs water
		{
			//open canal C 
			canal_C->toggleOpen(true);
			canal_C->setFlowRate(rate);
		} else 
		{
			canal_C->toggleOpen(false);
		}

		if(EastGain > 0 && NorthGain <= NorthNeed)
		{
			canal_B->toggleOpen(true);
			canal_B->setFlowRate(rate);
		}else
		{
			canal_B->toggleOpen(false);
		}

		// Update for next hour of simulation 
		manager.nexthour();
	}
}

/*
double gain(Region *r)
{
	double gain = 0.0;
	double difference = r->waterLevel - r->waterNeed;
	return (difference > 0.0) ? difference: 0.0;
}

double deficit(Region *r)
{
	double deficit = 0.0;
	double difference = r->waterNeed - r->waterLevel;
	return (difference > 0.0) ? difference:0.0;
}

//THOUSANDS of gallons of water are being moved around. Therefore, we need to develop an algorithm that will convert the water difference to corresponding flow rate.
double DetermineFlowRate(double RegionDeficit, double RegionGain)
{
	if(RegionDeficit <= 0.0 || RegionGain <= 0.0) return 0.0;

	double amountToMove = abs(RegionDeficit - RegionGain); //in acre-feet
	double rate = ((amountToMove * 325851)/3600) * 1000; //conversion to gal/s
	if(rate > 1.0) rate = 1.0; // Capping rate => 1 gal/s
	if(rate < 0.5) rate = 0.5; // Moderate flow
	return rate;
}


void solveProblems(AcequiaManager& manager)
{	
	auto canals = manager.getCanals();
	auto region = manager.getRegions();

	//Assigning regions 
	auto North = region[0];
	auto NorthNeed = region[0]->waterNeed;

	auto South = region[1];
	auto SouthNeed = region[1]->waterNeed;

	auto East = region[2];
	auto EastNeed = region[2]->waterNeed;

	//Assigning canal names based on initialization information from AcequiaManager.cpp
	auto canal_A = canals[0]; //Connecting North -> South
	auto canal_B = canals[1]; //Connecting South -> East
	auto canal_C = canals[2]; //Connecting North -> East
	auto canal_D = canals[3]; //Connecting East -> North


	while(!manager.isSolved && manager.hour != manager.SimulationMax)
	{
		// Opening canals before each iteration 
		for(auto canal : canals)
		{
			canal->toggleOpen(true);
		}

		// Updating current water level through each iteration 
		auto NorthCurrent = North->waterLevel;
		auto SouthCurrent = South->waterLevel;
		auto EastCurrent = East->waterLevel;

		auto NorthGain = gain(North);
		auto SouthGain = gain(South);
		auto EastGain = gain(East);

		auto NorthDeficit = deficit(North);
		auto SouthDeficit = deficit(South);
		auto EastDeficit = deficit(East);

		/*
			defecit[0] = North
			defecit[1] = South 
			defecit[2] = East
		*/
		// WHAT ACEQUIAMANAGER PROVIDES US:
		// North region is the key component for this system: it has canals connecting water to both South and East.
		// Need to make sure North has enough water to distribute. 
		// Canals are flowing in ONE DIRECTION. 
		// First priority is the North region.
/*
		//------NORTH-------
		if(NorthGain > 0.0 && SouthDeficit > 0)
		{
			//If South has a deficit, North sends water if it has gain through canal A
			if(SouthDeficit > 0)
			{
				canal_A->setFlowRate(DetermineFlowRate(SouthDeficit, NorthGain));
				//Stop once North reaches water level
			}

			//If East has a deficit, North sends water if it has gain through canal C
			if(EastDeficit > 0)
			{
				canal_C->setFlowRate(DetermineFlowRate(EastDeficit, NorthGain));
				//Stop once North reaches water level 
			} 

		} else if(NorthCurrent >= NorthNeed)
		{
			canal_C->toggleOpen(false);
			canal_A->toggleOpen(false);
		}

		//-------EAST-------
		if(EastGain > 0.0 && NorthDeficit > 0.0){
			//if North has deficit, East sends water if it has gain through canal D
			canal_D -> setFlowRate(DetermineFlowRate(NorthDeficit, EastGain));
			//stop once North reaches 0 deficit. 
		} else //if((NorthDeficit <= 0) && (EastCurrent > EastNeed))
		{
			canal_D -> toggleOpen(false);
		}

		//-----SOUTH-------
		if(SouthGain > 0.0 && EastDeficit > 0.0)
		{
			//if East as deficit, South can send water if it has gain through  canal B
			canal_B -> setFlowRate(DetermineFlowRate(EastDeficit, SouthGain));
			//stop once east reaches 0 deficit.
		} else //if((EastDeficit <= 0) && (SouthCurrent > SouthNeed))
		{
			//stop once South reaches water level
			canal_B ->toggleOpen(false);
		}

		// Update for next hour of simulation 
		manager.nexthour();
	}
}
*/