#pragma once
struct GTR2ShMem {
	float userInput[6];

	float rpm; //Radians per second
	float maxEngineRPM;
	float fuelPressure; //KPa
	float fuel; //Liters
	float fuelCapacityLiters;
	float engineWaterTemp;
	float engineOilTemp;
	float engineOilPressure;
	float carSpeed; //M/S
	
	long numberOfLaps;
	long completedLaps;

	float lapTimeBest; //Seconds
 	float lapTimePrevious;
	float lapTimeCurrent;
	long position;
	long numCars;

	long gear;

	float tirefrontLeft[3];	
	float tirefrontRight[3];
	float tirerearLeft[3];
	float tirerearRight[3];

	long numPenalties;

	float carCGLoc[3];

	float pitch;
	float yaw;
	float roll;

	float lateral;
	float vertical;
	float longitudinal;
};