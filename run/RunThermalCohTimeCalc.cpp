//
//  RunThermalCohTimeCalc.cpp
//  ToricCodeThermal
//
//  Created by Chris Self on 07/08/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSystem.h"

using namespace std;

int main(int argc, char* argv[]) {
    
    // check that inputs have been passed
    if ( argc<3 ) { 
        std::cerr << "Usage: " << argv[0] << " SYS_SIZE" << " RUN_NUM" << std::endl;
        
    } else {

	    // test parameters
	    int L = atoi(argv[1]);
		long num_trials = static_cast<long>(1E4);
	    double run_number = atoi(argv[2]);
	    
		// initialise file output junk
		ofstream outputf;
		std::string s; std::stringstream outstring;
	    outstring.str("");
	    outstring.clear();
	    outstring << "TCThermal_" << L << "_" << run_number << ".txt";
	    s=outstring.str();
	    const char * c = s.c_str();
	    outputf.open (c, std::ofstream::out | std::ofstream::app);
	    
	    CCSystem system = (L);
	    
	    // find data range:  run search algorithm to find an estimate of the interval containing the coherence time
		std::pair<double,double> coh_time_inteval = system.estimateThermalCoherenceTime( num_trials );
	    
		// take data:  plot 10 points across the range around the coherence time
		double end_time;
	    double success_rate;
		for ( int i=0; i<10; i++ ) {
			end_time  = coh_time_inteval.first + i*1.0*(coh_time_inteval.second-coh_time_inteval.first)/9.0;
	        success_rate =  system.calcThermalSuccessRate(end_time,num_trials);
			cout << CCSystem::beta << '\t' << end_time << '\t' << success_rate << '\n';
			outputf << CCSystem::beta << '\t' << end_time << '\t' << success_rate << '\n';
		}
	}
    
	return 0;
}
