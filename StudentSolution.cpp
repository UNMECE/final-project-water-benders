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
In this case, our gain will be the difference or the "gap" waterneed and the waterlevel. 
First, create our gain function.
*/
double gain(Region *r)
{
	double gain = r->waterNeed - r->waterLevel;
	return gain;
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
			//Open canal D 
			canal_D -> toggleOpen(true);
			canal_D->setFlowRate(rate);
		} else 
		{
			canal_D->toggleOpen(false);
		}

		//-----SOUTH-------
		if(SouthGain > 0) // If South needs water
		{
			//Open canal A		
			canal_A->toggleOpen(true);
			canal_A->setFlowRate(rate);	
		} else
		{
			canal_A->toggleOpen(false);
		}

		//-------EAST-------
		if(EastGain > 0)// If East needs water
		{
			//Open canal C 
			canal_C->toggleOpen(true);
			canal_C->setFlowRate(rate);
		} else 
		{
			canal_C->toggleOpen(false);
		}

		if(EastGain > 0 && NorthGain <= NorthNeed)// If South needs more water 
		{
			//Open canal B
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

