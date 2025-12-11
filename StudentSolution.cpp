#include "acequia_manager.h"
#include <iostream>
#include <cmath> 

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


//function for determining flowrate. There is 36000 s in an hour. amount_to_move is in acre-feet.
//if flow rate is 1, we are trasferring 1 * 3600 / 1000 = 3.6 acre-feet per hour (the maximum)
double CalculateFlowRate(Region* source, Region* destination)
{
	double flowrate;

	// Calculate how much water destination needs (acre-feet)
    double difference = destination->waterNeed - destination->waterLevel;
    
    // Flow should be proportional to deficit severity
    // If deficit is 50 acre-feet and we can transfer 3.6 acre-feet/hour max:
    // ratio = 50 / 50 = 1.0 → use full capacity
    
    double ratio = (difference / destination->waterNeed);  // 0.0 to 1.0+
    double flow_gal_per_sec = ratio * 100.0;
    // Convert to flowRate (0.0 to 1.0 scale)
    double flowRate = (flow_gal_per_sec * 3600) / 325851.0;
    
    // Cap at maximum flow rate.
    if(flowRate > 1.0) flowRate = 1.0;
    
    return flowRate;

}

void solveProblems(AcequiaManager& manager)
{
	auto canal = manager.getCanals();
	auto regions = manager.getRegions();

	auto North = regions[0];
	auto NorthCurrent = regions[0]->waterLevel;
	auto NorthNeed = regions[0]->waterNeed;

	auto South = regions[1];
	auto SouthCurrent = regions[1]->waterLevel;
	auto SouthNeed = regions[1]->waterNeed;

	auto East = regions[2];
	auto EastCurrent = regions[2]->waterLevel;
	auto EastNeed = regions[2]->waterNeed;

	while(!manager.isSolved && manager.hour != manager.SimulationMax)
	{
		//North region is the key component for this system: it has canals connecting water to both South and East.
		//Need to make sure North has enough water to distribute. 
		//East to North => logic for canal D. The only canal that can move water to North.
		if(NorthCurrent < NorthNeed && EastCurrent > EastNeed)
		{
			auto rate = CalculateFlowRate(East, North);
			canal[3]->setFlowRate(rate);
			canal[3]->toggleOpen(true);

		}else if(EastCurrent >= EastNeed) 
		{
			canal[3]->toggleOpen(false);//close canal once East reaches its water need
		}

		//Next, we can start transferring water to both South and East depending on North's healthy amount of water. 
		if(NorthCurrent > NorthNeed &&  NorthCurrent < North->waterCapacity) 
		{
			//North to South => logic for canal A
			if(SouthCurrent < SouthNeed)
			{
				auto rate = CalculateFlowRate(North, South);
				canal[0]->setFlowRate(rate);
				canal[0]->toggleOpen(true);
			}

			//North to East => logic for canal C
			if(EastCurrent < EastNeed)
			{
				auto rate = CalculateFlowRate(North, East);
				canal[3]->setFlowRate(rate);
				canal[3]->toggleOpen(true);

			}
		}else if(NorthCurrent == NorthNeed)
		{
			//Once North reaches its water needed, stop canal A and C 
			canal[3]->toggleOpen(false);
			canal[0]->toggleOpen(false);
		}
				

		//South to East => logic for canal B 
		if(EastCurrent > EastNeed && SouthCurrent < SouthNeed)
		{
			auto rate = CalculateFlowRate(South, East);
			canal[1]->setFlowRate(rate);
			canal[1]->toggleOpen(true);
		} else if(SouthCurrent == SouthNeed )
		{
			canal[1]->toggleOpen(false);
		}

		if(EastCurrent <= EastNeed)
		{
			canal[3]->toggleOpen(false);
		}

		

		manager.nexthour();
	}

}


//In this solution, students can make functions that aid in identifying the best course of action for moving
//water resources. They can set conditions that then work on the canal vectors based on the information reported

//This can help in optimizing solutions for dynamic constraints such as weather (rain, or dried up waterSources) and
//make the solution to the problem more abstract, allowing the logic and algorithm to be the sole priority of the student
//while the computation is left for the Acequia Manager

//This would be the perfect opportunity to identify the tools learned from ECE 231L such as:
//data structures (stacks, queues, trees(?)), templates, vector class functions, etc... to aid in the algorithm solution


/*
int findCanal(std::vector<Canal *> canals, std::string region)
{
	int match;
	for(int i = 0; i < canals.size(); i++)
	{
		if(canals[i]->sourceRegion->name == region)
		{
			match = i;
		}
	}
	return match;
}

void release(std::vector<Canal *> canals, std::string region)
{
	int match = findCanal(canals, region);
	canals[match]->toggleOpen(true);
	canals[match]->setFlowRate(1);
}

void close(std::vector<Canal *> canals, std::string region)
{
	int match = findCanal(canals, region);
	canals[match]->toggleOpen(false);
}


void solveProblems(AcequiaManager& manager)
{

	bool check = false;
	auto canals = manager.getCanals();
	auto regions = manager.getRegions();

	while(!manager.isSolved && manager.hour!=manager.SimulationMax)
	{
		
		if(manager.hour == 0)
		{
			for(int i = 0; i < canals.size(); i++)
			{
				canals[i]->toggleOpen(true);
				canals[i]->setFlowRate(1);
			}
		}

		for(int i = 0; i < regions.size(); i++)
		{
			if(regions[i]->isFlooded == true)
			{
				//release water from that region by a canal
				release(canals, regions[i]->name);
			}
			else if(regions[i]->isInDrought = true)
			{
				//find canal to move water
				close;
			}

			else if(regions[i]->isFlooded == true && regions[i]->isInDrought == true)
			{
				close(canals, regions[i]->name);
			}
		}
		
		manager.nexthour();
	}
}

*/