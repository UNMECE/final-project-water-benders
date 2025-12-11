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


/*example 2 format*/

/*void solveProblems(AcequiaManager& manager)
{
	auto canals = manager.getCanals();
	while(!manager.isSolved && manager.hour!=manager.SimulationMax)
	{
	//Students will implement this function to solve the probelms
	//Example: Adjust canal flow rates and directions

	//First of all, this is a fixed scenario. 
		if(manager.hour==0)
		{
			canals[0]->setFlowRate(1);
			canals[0]->toggleOpen(true);
		}
		else if(manager.hour==1)
		{
			canals[1]->setFlowRate(0.5);
			canals[1]->toggleOpen(true);
		}
		else if(manager.hour==82)
		{
			canals[0]->toggleOpen(false);
			canals[1]->toggleOpen(false);
		}

		//only add solution here!!!


	//student may add any necessary functions or check on the progress of each region as the simulation moves forward. 
	//The manager takes care of updating the waterLevels of each region and waterSource while the student is just expected
	//to solve how to address the state of each region

		
		manager.nexthour();
	}
*/

/*
void solveProblems(AcequiaManager& manager)
{
	//Accessing canals and regions 
    auto canals = manager.getCanals();
    auto regions = manager.getRegions();
    
	//Use canals to move the water 
    while(!manager.isSolved && manager.hour != manager.SimulationMax)
    {
		
		
		manager.nexthour();
	}

	// Phase 3: Lock solution to prevent further changes
	for(auto canal : canals)
		canal->toggleOpen(false);
}
*/

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

	double amountToMove = std::min(RegionDeficit, RegionGain);
	double rate = amountToMove / 3.6;
	if(rate > 1.0) rate = 1.0; // Capping rate
	if(rate < 0.8) rate = 0.5; // Moderate flow
	if(rate < 0.5) rate = 0.01; //smallest flow 
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
	auto canalA = canals[0]; //Connecting North -> South
	auto canalB = canals[1]; //Connecting South -> East
	auto canalC = canals[2]; //Connecting North -> East
	auto canalD = canals[3]; //Connecting East -> North


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

		//------NORTH-------
		if(NorthGain > 0.0 || !North->isInDrought)
		{
			//If South has a deficit, North sends water if it has gain through canal A
			if(SouthDeficit > 0)
			{
				canalA->setFlowRate(DetermineFlowRate(SouthDeficit, NorthGain));
				//Stop once North reaches water level
				if(SouthDeficit <= 0 && SouthCurrent > SouthNeed)
				{
					canalA->toggleOpen(false);
				}
			}

			//If East has a deficit, North sends water if it has gain through canal C
			if(EastDeficit > 0)
			{
				canalC->setFlowRate(DetermineFlowRate(EastDeficit, NorthGain));
				//Stop once North reaches water level (with tolerance for floating-point)
				// if(NorthCurrent == NorthNeed)
				// {
				// 	canalA->toggleOpen(false);
				// }
			}

		}

		//-------EAST-------
		if(EastGain > 0.0 || !East->isInDrought){

			//if North has deficit, East sends water if it has gain through canal D
			if(NorthDeficit > 0)
			{
				canalD -> setFlowRate(DetermineFlowRate(NorthDeficit, EastGain));
				//stop once east reaches 0 deficit
			}
		}

		//-----SOUTH-------
		if(SouthGain > 0.0 || !South->isInDrought)
		{
			//if East as deficit, South can send water if it has gain through  canal B
			if(EastDeficit > 0)
			{
				canalB -> setFlowRate(DetermineFlowRate(EastDeficit,SouthGain));
				//stop once South reaches water level
				if(EastDeficit <= 0 && EastCurrent > EastNeed)
				{
					canalB->toggleOpen(false);
				}
			}
		}

		// Update for next hour of simulation 
		manager.nexthour();
	}

	for(auto canal : canals)
	{
		canal->toggleOpen(false);
	}

}


