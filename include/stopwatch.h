//
//  stopwatch.h
//  ColorCodeDynamicsSimulation
//
//  Created by Chris Self on 25/03/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#ifndef ColorCodeDynamicsSimulation_stopwatch_h
#define ColorCodeDynamicsSimulation_stopwatch_h

class stopwatch 
{
private:
	clock_t startTime, stopTime;

public:
	stopwatch() { startTime = clock(); }
    
	void reset() { startTime = clock(); }
	void stop() { stopTime = clock(); }
    
	double getElapsed() {
		stopTime = clock();
		return (static_cast<double>(stopTime - startTime))/(static_cast<double>(CLOCKS_PER_SEC));
		// return ((long int)(stopTime - startTime));
	}

	double readElapsed() {
		return (static_cast<double>(stopTime - startTime))/(static_cast<double>(CLOCKS_PER_SEC));
		// return ((long int)(stopTime - startTime));
	}
};

#endif