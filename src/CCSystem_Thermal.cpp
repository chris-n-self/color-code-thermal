//
//  CCSystem.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSystem.h"

// function to calculate nCr
//
double CombinationsIter(long iN, long iR) {
    if (iR < 0 || iR > iN) {
        return 0;
    }
    double iComb = 1.0;
    long i = 0;
    while (i < iR) {
        ++i;
        iComb *= static_cast<double>(iN - i + 1);
        iComb /= static_cast<double>(i);
    }
    return iComb;
}

// out of line definitions of thermal variables
//
double gammaFunction( double omega_ );
const double CCSystem::beta = static_cast<double>( 30 )/10.0;

/*
 ---------------
 initialisations
 ---------------
 */

void CCSystem::setThermalRates() {
    thermal_rates[0] = gammaFunction(-3.0);
    thermal_rates[1] = gammaFunction(-1.0);
    thermal_rates[2] = gammaFunction(1.0);
    thermal_rates[3] = gammaFunction(3.0);
}

double gammaFunction( double omega_ ) {
    return omega_/( 1.0 - exp(-CCSystem::beta*omega_) );
}

/*
 ------------------------------
 steps of the thermal evolution
 ------------------------------
 */

void CCSystem::incrementTime() {
    
    // this check avoids a problem I had once with negative time steps
    /* double time_step = -log( rand_gen() )/total_rate;
     while ( time_step < 0 ) {
     time_step = -log( rand_gen() )/total_rate;
     }
     simulation_time += time_step; */
    
    simulation_time += (-log( rand_gen() )/total_rate);
}

void CCSystem::thermalSpinFlip() {
    /*
     then we iterate over the spins keeping a cumulative tally of the rate, when the next spin would push
     the tally over the random # we flip that spin
     */
    double rand_rate_target = total_rate*rand_gen();
    double ratetally = 0.0;
    for ( auto& spin_row : spins ) {
        for ( auto& spin_plaq : spin_row ) {
            for ( auto& spin : spin_plaq ) {
                ratetally += thermal_rates[ spin.second.getRateKey() ];
                if ( ratetally > rand_rate_target ) {
                    spin.second.flipState();
                    // std::cout << "Flipped spin " << spin.second.x() << ", " << spin.second.y() << ", " << spin.second.idNum() << " to " << spin.second.getState() << '\n';
                    
                    // update syndromes changed by this flip
                    int x = spin.second.x();
                    int y = spin.second.y();
                    if ( spin.second.idNum() == 1 ) {
                        // spin_plaq.first -> (x,y), (x-1,y), (x-1,y+1)
                        handleStabiliserChange(x,y);
                        handleStabiliserChange((x+sys_size-1)%sys_size,y);
                        handleStabiliserChange((x+sys_size-1)%sys_size,(y+1)%sys_size);
                    } else {
                        // spin_plaq.second -> (x,y), (x-1,y+1), (x,y+1)
                        handleStabiliserChange(x,y);
                        handleStabiliserChange((x+sys_size-1)%sys_size,(y+1)%sys_size);
                        handleStabiliserChange(x,(y+1)%sys_size);
                    }
                    
                    // drawAnimationFrame(dout,spin.second.x(),spin.second.y(),spin.second.idNum());
                    
                    // update thermal rates
                    updateLocalRates(spin.second.x(),spin.second.y(),spin.second.idNum());
                    total_rate = 0.0;
                    for ( auto rates_class : rates_class_counts ) {
                        total_rate += rates_class.second*thermal_rates[rates_class.first];
                    }
                    
                    return;
                }
            }
        }
    }
}

void CCSystem::handleStabiliserChange( int x_, int y_ ) {
    stabilisers[x_][y_].flipOccupation();
    if( stabilisers[x_][y_].getOccupation() ) {
        anyon_positions.insert( std::make_pair(x_,y_) );
    } else {
        anyon_positions.erase( std::make_pair(x_,y_) );
    }
}

void CCSystem::updateLocalRates( int x_, int y_, int id_ ) {
    
    updateSpinRate( x_, y_, 1 ); // <--- (x,y,#1)
    updateSpinRate( x_, y_, 2 ); // <--- (x,y,#2)
    updateSpinRate( (x_+(sys_size-1))%sys_size, (y_+1)%sys_size, 1 ); // <--- (x-1,y+1,#1)
    updateSpinRate( (x_+(sys_size-1))%sys_size, (y_+1)%sys_size, 2 ); // <--- (x-1,y+1,#2)
    updateSpinRate( (x_+1)%sys_size, (y_+(sys_size-1))%sys_size, 1 ); // <--- (x+1,y-1,#1)
    updateSpinRate( (x_+1)%sys_size, (y_+(sys_size-1))%sys_size, 2 ); // <--- (x+1,y-1,#2)
    updateSpinRate( (x_+1)%sys_size, y_, 1 ); // <--- (x+1,y,#1)
    updateSpinRate( (x_+(sys_size-1))%sys_size, y_, 2 ); // <--- (x-1,y,#2)
    updateSpinRate( x_, (y_+1)%sys_size, 1 ); // <--- (x,y+1,#1)
    updateSpinRate( x_, (y_+(sys_size-1))%sys_size, 2 ); // <--- (x,y-1,#2)
    
    if ( id_ == 1 ) {
        
        updateSpinRate( x_, (y_+1)%sys_size, 1 ); // <--- (x,y+1,#1)
        updateSpinRate( x_, (y_+1)%sys_size, 2 ); // <--- (x,y+1,#2)
        updateSpinRate( (x_+1)%sys_size, (y_+1)%sys_size, 2 ); // <--- (x+1,y+1,#2)
        
        return;
    } else {

        updateSpinRate( (x_+(sys_size-1))%sys_size, y_, 1 ); // <--- (x-1,y,#1)
        updateSpinRate( (x_+(sys_size-1))%sys_size, y_, 2 ); // <--- (x-1,y,#2)
        updateSpinRate( (x_+(sys_size-1))%sys_size, (y_+(sys_size-1))%sys_size, 1 ); // <--- (x-1,y-1,#1)
        
        return;
    }
}

void CCSystem::updateSpinRate( int x_, int y_, int id_ ) {
    
    rates_class_counts[ spins[x_][y_][id_].getRateKey() ]--;
    
    if( id_==1 ) {
        spins[x_][y_][id_].setRateKey( static_cast<int>(stabilisers[x_][y_].getOccupation())+static_cast<int>(stabilisers[(x_+(sys_size-1))%sys_size][(y_+1)%sys_size].getOccupation())+static_cast<int>(stabilisers[(x_+(sys_size-1))%sys_size][y_].getOccupation()) );
    } else {
        spins[x_][y_][id_].setRateKey( static_cast<int>(stabilisers[x_][y_].getOccupation())+static_cast<int>(stabilisers[(x_+(sys_size-1))%sys_size][(y_+1)%sys_size].getOccupation())+static_cast<int>(stabilisers[x_][(y_+1)%sys_size].getOccupation()) );
    }
    // std::cout << "spin rate " << x_ << ", " << y_ << ", " << id_ << " to " << spins[x_][y_][1].getRateKey() << '\n';
    
    rates_class_counts[ spins[x_][y_][id_].getRateKey() ]++;
}

/*
 ---------------------------
 thermal evolution algorithm
 ---------------------------
 */

void CCSystem::thermalEvolution( double end_time_ ) {
    incrementTime();
    while( simulation_time < end_time_ ) {
        thermalSpinFlip();
        incrementTime();
    }
}

/*
 -----------------------
 thermal noise functions
 -----------------------
 */

double CCSystem::calcThermalSuccessRate( double end_time_, long int num_trials_ ) {
    long int success_count = 0;
    
    for ( long int i=0; i<num_trials_; i++ ) {
        resetSystem();
        thermalEvolution(end_time_);
        if ( runDecoder() ) { success_count++; }
    }
    
    return (static_cast<double>(success_count))/(static_cast<double>(num_trials_));
}

double CCSystem::calcThermalSuccessRateFailFast( double end_time_, long num_trials_ ) {
    long success_count = 0;
    
    for ( long block=0; block<10; block++ ) {
        for ( long i=0; i<(num_trials_/10); i++ ) {
            resetSystem();
            thermalEvolution(end_time_);
            if ( runDecoder() ) { success_count++; }
        }
        
        // HYPOTHESIS TEST
        // calculate probability of observed at least this number of fails assuming p=0.9
        // if this prob < 0.05 fail fast.
        //
        long number_so_far = (block+1)*(num_trials_/10);
        double lower_hyp_test_prob = 0.0;
        for ( int j=0; j<(success_count+1); ++j ) {
            lower_hyp_test_prob += static_cast<double>(CombinationsIter( number_so_far, j ))*pow(0.9,static_cast<double>(j))*pow(0.1,static_cast<double>(number_so_far-j));
        }
        if (lower_hyp_test_prob<0.05) {
            std::cout << "@ " << end_time_ << " and N=" << number_so_far << " could say with confidence " << (1-lower_hyp_test_prob) << " that p<0.9\n";
            return 0.9;
        }
    }
    
    double success_rate = (static_cast<double>(success_count))/(static_cast<double>(num_trials_));
    std::cout << "@ " << end_time_ << '\t' << success_rate << '\n';
    return success_rate;
}

std::pair<double,double> CCSystem::estimateThermalCoherenceTime( long int number_trials_ ) {
    double end_time = 0.0;
    // std::cout.precision(10);
    // std::cout.setf( std::ios::fixed, std:: ios::floatfield );
    
    /*
     test the initial time success rate to see if the search needs to begin by decreasing or increasing the end time
     */
    double success_rate;
    success_rate = calcThermalSuccessRateFailFast( end_time, number_trials_/10 );
    
    double interval_start, interval_end;
    /*
     search to find the scale that the coherence time is on
     */
    int scale = -3;
    for ( int i = -3; i<30; ++i ) {
        scale = i;
        end_time = 1.0*pow( 10.0, scale*1.0 );
        success_rate = calcThermalSuccessRateFailFast( end_time, number_trials_/10 );
        
        /*
         stop condition is the search has the scale of the coherence time
         */
        if ( success_rate<(1.0-(1.0/(number_trials_/10))) ) {
            break;
        }
    }
    
    /*
     if the scale search has overshot the coherence time we need to search backward first
     */
    double search_multiplier = 5.0;
    if ( success_rate<0.994 ) {
        scale--;
    }
    while ( success_rate<0.994 ) {
        end_time -= search_multiplier*1.0*pow( 10.0, scale*1.0 );
        success_rate = calcThermalSuccessRateFailFast( end_time, number_trials_/10 );
        search_multiplier = search_multiplier/2.0;
    }
    
    /*
     find the 99.4-99.6% lower bound of the interval
     */
    interval_start = end_time;
    search_multiplier = 1.0;
    while ( (success_rate>0.996) || (success_rate<0.994) ) /* while outside the  */ {
        end_time += search_multiplier*1.0*pow( 10.0, scale*1.0 );
        success_rate = calcThermalSuccessRateFailFast( end_time, number_trials_/10 );
        if ( !(success_rate<0.994) ) /* if greater than or equal to the lower bound */ {
            interval_start = end_time;
            search_multiplier = search_multiplier*2.0;
        } else {
            end_time -= search_multiplier*1.0*pow( 10.0, scale*1.0 );
            search_multiplier = search_multiplier/2.0;
        }
    }
    std::cout << "interval_start set @ " << end_time << '\n';
    
    /*
     find the 98.4-98.6% upper bound of the interval
     */
    interval_end = end_time;
    if ( search_multiplier < 1.0 ) {
        search_multiplier = 1.0;
    }
    while ( (success_rate>0.986) || (success_rate<0.984) ) {
        end_time += search_multiplier*1.0*pow( 10.0, scale*1.0 );
        success_rate = calcThermalSuccessRateFailFast( end_time, number_trials_/10 );
        if ( !(success_rate<0.984) ) /* if greater than or equal to the lower bound */ {
            interval_end = end_time;
            search_multiplier = search_multiplier*2.0;
        } else {
            end_time -= search_multiplier*1.0*pow( 10.0, scale*1.0 );
            search_multiplier = search_multiplier/2.0;
        }
    }
    std::cout << "interval_end set @ " << end_time << '\n';
    
    std::cout << "return interval " << interval_start << '\t'  << interval_end << '\n' << '\n';
    return std::make_pair( interval_start, interval_end );
}

