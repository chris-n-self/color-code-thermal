//
//  RunPlayground.cpp
//  ToricCodeThermal
//
//  Created by Chris Self on 07/08/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSystem.h"

using namespace std;

int main(int argc, char * argv[]) {

    // check that inputs have been passed
    if ( argc<3 ) { 
        std::cerr << "Usage: " << argv[0] << " SYS_SIZE" << std::endl;
        
    } else {

        // data output streams
    	ofstream drawing;
    	std::string s; std::stringstream outstring;
        // test drawing
    	outstring.str("");
    	outstring.clear();
    	outstring << "test_drawing4.asy";
    	s=outstring.str();
    	const char * c1 = s.c_str();
    	drawing.open (c1, std::ofstream::out | std::ofstream::trunc);

        CCSystem cc_system(atoi(argv[1]));
        
        cc_system.sufferIIDNoise(0.07);
        // cc_system.flipSpin(0,19,1);
        // cc_system.flipSpin(0,19,2);
        // cc_system.evaluateSyndromes();
        std::cout <<  std::boolalpha << cc_system.runDecoder() << '\n';
        cc_system.drawAll(drawing);
        // cc_system.drawLookupTable(7,7,drawing);
        
        cc_system.testBoxParticleMatching();
        
        // double succ_rate = cc_system.calcIIDSuccessRate(mt_rand_generator, 0.05, 1000);
        // std::cout << succ_rate ;
        
        // cc_system.drawAnimationHeader(drawing);
        // cc_system.thermalEvolution(200);
        // cc_system.drawAnimationFooter(drawing);
    }
    
    return 0;
}