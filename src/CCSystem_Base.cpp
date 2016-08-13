//
//  CCSystem.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSystem.h"

// out of line defintion of random number generator
//
MTRand_closed CCSystem::rand_gen(time(0));
//
CCSystem::CCSystem( int L_ ) {
    sys_size = 3*L_;
    
    // initialise spins
    spins.resize(sys_size);
    for (int x=0; x<sys_size; x++) {
        // inner dimension is physical X, outer dimension is physical X
        spins[x].resize(sys_size);
        for (int y=0; y<sys_size; y++) {
            spins[x][y][1] = CCSpin(x,y,1);
            spins[x][y][2] = CCSpin(x,y,2);
        }
    }
    
    // initialise stabilisers
    stabilisers.resize(sys_size);
    for (int x=0; x<sys_size; x++) {
        // inner dimension is physical X, outer dimension is physical X
        stabilisers[x].resize(sys_size);
        for (int y=0; y<sys_size; y++) {
            int color_int = 1 + (x + (sys_size-y)%3)%3;
            stabilisers[x][y] = CCStabiliser(x,y,color_int);
        }
    }
    
    logical_state = std::make_tuple(false, false, false, false);
    buildDecoderLookupTable();
    
    // initialise thermal properties
    simulation_time = 0.0;
    setThermalRates();
    total_rate = 2*sys_size*sys_size*thermal_rates[0];
    rates_class_counts[0] = 2*sys_size*sys_size;
    rates_class_counts[1] = 0;
    rates_class_counts[2] = 0;
    rates_class_counts[3] = 0;
    
    // initialise spin rate key
    for ( auto& spin_row : spins ) {
        for ( auto& spin_plaq : spin_row ) {
            for ( auto& spin : spin_plaq ) {
                spin.second.setRateKey(0);
            }
        }
    }
}
/*
 ----------------
 lattice geometry
 ----------------
 */
std::list<std::pair<int,int>> CCSystem::getNeighbours( int from_x_, int from_y_ ) {
    std::list<std::pair<int,int>> neighbours;
    neighbours.push_back( std::make_pair( (from_x_+1)%sys_size, (from_y_)%sys_size) );
    neighbours.push_back( std::make_pair( (from_x_)%sys_size, (from_y_+1)%sys_size ) );
    neighbours.push_back( std::make_pair( (from_x_+(sys_size-1))%sys_size, (from_y_)%sys_size ) );
    neighbours.push_back( std::make_pair( (from_x_)%sys_size, (from_y_+(sys_size-1))%sys_size ) );
    neighbours.push_back( std::make_pair( (from_x_+1)%sys_size, (from_y_+(sys_size-1))%sys_size ) );
    neighbours.push_back( std::make_pair( (from_x_+(sys_size-1))%sys_size, (from_y_+1)%sys_size ) );
    return neighbours;
}

std::vector<std::pair<int,int>> CCSystem::getLeveliNeighbours( int level_i_, int from_x_, int from_y_ ) {
    std::vector<std::pair<int,int>> leveliNeighbours;
    auto leveliOffsets = decoder_lookup_table.at(level_i_);
    for ( auto offset : leveliOffsets ) {
        leveliNeighbours.push_back( std::make_pair( (offset.first+from_x_)%sys_size, (offset.second+from_y_)%sys_size ) );
    }
    return leveliNeighbours;
    
}
/*
 ---------------------
 measurement processes
 ---------------------
 */
void CCSystem::evaluateSyndromes() {
    
    /*
     delete existing syndrome information
     */
    for ( auto& stabil_row : stabilisers ) {
        for ( auto& stabil : stabil_row ) {
            stabil.setOccupation(false);
        }
    }
    
    /*
     a method for evaluating the full set of syndromes of the system that visits every spin only once and distributes the
     parity information to the affected stabilisers
     */
    for ( auto spin_row : spins ) {
        for ( auto spin_plaq : spin_row ) {
            int x = spin_plaq[1].x();
            int y = spin_plaq[1].y();
            
            // spin_plaq.first -> (x,y), (x-1,y), (x-1,y+1)
            stabilisers[x][y].setOccupation( stabilisers[x][y].getOccupation()^spin_plaq[1].getState() );
            stabilisers[(x+sys_size-1)%sys_size][y].setOccupation( stabilisers[(x+sys_size-1)%sys_size][y].getOccupation()^spin_plaq[1].getState() );
            stabilisers[(x+sys_size-1)%sys_size][(y+1)%sys_size].setOccupation( stabilisers[(x+sys_size-1)%sys_size][(y+1)%sys_size].getOccupation()^spin_plaq[1].getState() );
            
            // spin_plaq.second -> (x,y), (x-1,y+1), (x,y+1)
            stabilisers[x][y].setOccupation( stabilisers[x][y].getOccupation()^spin_plaq[2].getState() );
            stabilisers[(x+sys_size-1)%sys_size][(y+1)%sys_size].setOccupation( stabilisers[(x+sys_size-1)%sys_size][(y+1)%sys_size].getOccupation()^spin_plaq[2].getState() );
            stabilisers[x][(y+1)%sys_size].setOccupation( stabilisers[x][(y+1)%sys_size].getOccupation()^spin_plaq[2].getState() );
        }
    }
    
    /*
     iterate back over the stabilisers and note down any which carry defects
     */
    findAnyons();
}

void CCSystem::findAnyons() {
    anyon_positions.clear();
    for ( auto stabil_row : stabilisers ) {
        for ( auto stabil : stabil_row ) {
            if ( stabil.getOccupation() ) {
                anyon_positions.insert(std::make_pair(stabil.x(), stabil.y()));
                // std::cout << "found anyon @ (" << stabil.x() << "," << stabil.y() << ")\n";
            }
        }
    }
}

void CCSystem::measureLogicalState() {
    std::get<0>(logical_state) = false;
    std::get<1>(logical_state) = false;
    std::get<2>(logical_state) = false;
    std::get<3>(logical_state) = false;
    
    for ( int s=0; s<sys_size; ++s ) {
        if ( (s%3)==0 ) {
            
            // logical_state[0]
            std::get<0>(logical_state) = std::get<0>(logical_state)^spins[s][0][1].getState();
            std::get<0>(logical_state) = std::get<0>(logical_state)^spins[s][0][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << 0 << ", " << 1 << ", \"0\" );\n";
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << 0 << ", " << 2 << ", \"0\" );\n";
            // logical_state[1]
            std::get<1>(logical_state) = std::get<1>(logical_state)^spins[sys_size-1][s][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << sys_size-1 << ", " << s << ", " << 2 << ", \"1\" );\n";
            // logical_state[2]
            std::get<2>(logical_state) = std::get<2>(logical_state)^spins[s][sys_size-2][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << sys_size-2 << ", " << 2 << ", \"2\" );\n";
            // logical_state[3]
            std::get<3>(logical_state) = std::get<3>(logical_state)^spins[1][s][1].getState();
            std::get<3>(logical_state) = std::get<3>(logical_state)^spins[1][s][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << 1 << ", " << s << ", " << 1 << ", \"3\" );\n";
            //dout << std::boolalpha << "placeMeasurement( " << 1 << ", " << s << ", " << 2 << ", \"3\" );\n";
            
        } else if ( (s%3)==1 ) {
            
            // logical_state[0]
            std::get<0>(logical_state) = std::get<0>(logical_state)^spins[s][0][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << 0 << ", " << 2 << ", \"0\" );\n";
            // logical_state[1]
            std::get<1>(logical_state) = std::get<1>(logical_state)^spins[sys_size-1][s][1].getState();
            //dout << std::boolalpha << "placeMeasurement( " << sys_size-1 << ", " << s << ", " << 1 << ", \"1\" );\n";
            // logical_state[2]
            std::get<2>(logical_state) = std::get<2>(logical_state)^spins[s][sys_size-2][1].getState();
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << sys_size-2 << ", " << 1 << ", \"2\" );\n";
            // logical_state[3]
            std::get<3>(logical_state) = std::get<3>(logical_state)^spins[1][s][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << 1 << ", " << s << ", " << 2 << ", \"3\" );\n";
            
        } else if ( (s%3)==2 ) {
            
            // logical_state[0]
            std::get<0>(logical_state) = std::get<0>(logical_state)^spins[s][0][1].getState();
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << 0 << ", " << 1 << ", \"0\" );\n";
            // logical_state[1]
            std::get<1>(logical_state) = std::get<1>(logical_state)^spins[sys_size-1][s][1].getState();
            std::get<1>(logical_state) = std::get<1>(logical_state)^spins[sys_size-1][s][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << sys_size-1 << ", " << s << ", " << 1 << ", \"1\" );\n";
            //dout << std::boolalpha << "placeMeasurement( " << sys_size-1 << ", " << s << ", " << 2 << ", \"1\" );\n";
            // logical_state[2]
            std::get<2>(logical_state) = std::get<2>(logical_state)^spins[s][sys_size-2][1].getState();
            std::get<2>(logical_state) = std::get<2>(logical_state)^spins[s][sys_size-2][2].getState();
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << sys_size-2 << ", " << 1 << ", \"2\" );\n";
            //dout << std::boolalpha << "placeMeasurement( " << s << ", " << sys_size-2 << ", " << 2 << ", \"2\" );\n";
            // logical_state[3]
            std::get<3>(logical_state) = std::get<3>(logical_state)^spins[1][s][1].getState();
            //dout << std::boolalpha << "placeMeasurement( " << 1 << ", " << s << ", " << 1 << ", \"3\" );\n";
            
        } else {
            std::cout << "ERROR: something has gone wrong getting logical state\n";
        }
    }
}
/*
 ---------------
 noise functions
 ---------------
 */
void CCSystem::resetSystem() {
    
    // reset spin states
    for ( auto& spin_row : spins ) {
        for ( auto& spin_plaq : spin_row ) {
            for ( auto& spin : spin_plaq ) {
                spin.second.setState(false);
            }
        }
    }
    
    // reset syndrome info and logical state
    evaluateSyndromes();
    measureLogicalState();
    
    // reset thermal properties
    simulation_time = 0.0;
    total_rate = 2*sys_size*sys_size*thermal_rates[0];
    rates_class_counts[0] = 2*sys_size*sys_size;
    rates_class_counts[1] = 0;
    rates_class_counts[2] = 0;
    rates_class_counts[3] = 0;
    
    // reset spin rate key
    for ( auto& spin_row : spins ) {
        for ( auto& spin_plaq : spin_row ) {
            for ( auto& spin : spin_plaq ) {
                spin.second.setRateKey(0);
            }
        }
    }
}

void CCSystem::flipSpin( int at_x_, int at_y_, int at_id_ ) {
    spins[at_x_][at_y_][at_id_].flipState();
}
//
// iid noise
//
void CCSystem::sufferIIDNoise( double error_rate_ ) {
    double uniform_rand;
    
    // iterate over all spins
    for ( auto& spin_row : spins ) {
        for ( auto& spin_plaq : spin_row ) {
            for ( auto& spin : spin_plaq ) {
                
                // flip if uniform rand < error rate
                uniform_rand = rand_gen();
                if ( uniform_rand<error_rate_ ) {
                    spin.second.flipState();
                    // std::cout << "flipped spin @ (" << spin.second.x() << "," << spin.second.y() << "," << spin.second.idNum() << ")\n";
                }
            }
        }
    }
    
    evaluateSyndromes();
}

double CCSystem::calcIIDSuccessRate( double error_rate_, long num_trials_ ) {
    long success_count = 0;
    
    for ( long i=0; i<num_trials_; i++ ) {
        resetSystem();
        sufferIIDNoise(error_rate_);
        if ( runDecoder() ) { success_count++; }
    }
    
    return (static_cast<double>(success_count))/(static_cast<double>(num_trials_));
}
