//
//  CCSystem.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSystem.h"

void CCSystem::buildDecoderLookupTable() {
    
    std::set<std::pair<int,int>> got_sites; // a list of all the, level i-1 and lower, sites already added to the look-up table
    std::set<std::pair<int,int>> current_level_got_sites; // a list of all the level i sites already accepted
    std::set<std::pair<int,int>> current_level_rejected_sites; // a list of all the level i sites already inspected and rejected
    std::list<std::pair<int,int>> current_level_list; // the ordered list of level i sites to go into the look-up table
    std::list<std::pair<int,int>> current_level_degenerate_list; // the ordered list of level i(degenerate) sites to go into the look-up table
    std::set<std::pair<int,int>> sites_to_search_around;
    
    int level = 1; // counter to track the current level
    /*
     Variable below is the position of the stabiliser that the code is looking around, I.E. at any stage while running the
     program is looking at the neighbours of the stabiliser at 'searchAround'
     */
    std::pair<int,int> search_around = std::make_pair( 0,0 );
    got_sites.insert(search_around);
    
    // record color at the centre
    int centre_color = stabilisers[0][0].color();
    
    std::cout << "BEGINNING TO BUILD DECODER LOOK-UP TABLE...\n";
    
    // LEVEL 1
    auto to_inspect = getNeighbours( search_around.first, search_around.second ); // list of the neighbours of 'searchAround'
    
    // iterate over all of the neighbours of 'searchAround'
    for ( auto site : to_inspect ) {
        /*
         since this is an odd distance from the central stabiliser want the stabilisers of different color to the
         centre of the search
         */
        if ( !( stabilisers[site.first][site.second].color() == centre_color ) ){
            current_level_list.push_back( site );
            current_level_got_sites.insert( site );
        } else {
            current_level_rejected_sites.insert( site );
        }
    }
    
    /*
     add all the sites identified as distance 1 sites to the cumulative list of accepted sites.  Then add the level 1
     list into the decoder look-up table
     */
    got_sites.insert(current_level_got_sites.begin(), current_level_got_sites.end());
    decoder_lookup_table.insert( std::make_pair( level, current_level_list ) );
    
    std::cout << "LEVEL 1 -- ADDED " << current_level_list.size() << " SITES.\n";
    
    // LEVEL i
    sites_to_search_around.insert(decoder_lookup_table[1].begin(), decoder_lookup_table[1].end());
    
    level++;
    do {
        
        // clear the temporary data from the previous level
        current_level_got_sites.clear();
        current_level_list.clear();
        current_level_degenerate_list.clear();
        current_level_rejected_sites.clear();
        
        // iterate over all the sites at the previous level, (i-1)
        for ( auto base_site : sites_to_search_around ) {
            
            // we will inspect all the neighbours of 'searchAround'
            search_around = base_site;
            to_inspect = getNeighbours( search_around.first, search_around.second );
            
            // iterate over all of the neighbours of 'searchAround'
            for ( auto inspect_site : to_inspect ) {
                
                // check that the site doesn't belong to a lower level, or has already been rejected at this level
                if ( !( (got_sites.count(inspect_site)>0) || (current_level_rejected_sites.count(inspect_site)>0) ) ) {
                    
                    // if i is even
                    if ( (level-2)%3==0 ) {
                        
                        /*
                         accept sites that are the same color as the centre stabiliser, if they have not been accepted
                         already.  If these conditions are not met record that this site has been rejected
                         */
                        if ( ( stabilisers[inspect_site.first][inspect_site.second].color() == centre_color ) && !(current_level_got_sites.count(inspect_site)>0) ){
                            current_level_list.push_back( inspect_site );
                            current_level_got_sites.insert( inspect_site );
                        } else {
                            current_level_rejected_sites.insert( inspect_site );
                        }
                    }
                    
                    // if i is odd
                    else {
                        
                        /*
                         accept sites that are a different color to the centre stabiliser, if this conditions is not
                         met record that this site has been rejected
                         */
                        if ( !( stabilisers[inspect_site.first][inspect_site.second].color() == centre_color ) ){
                            
                            /*
                             in order to handle degeneracy -- degenerate sites are sites that neighbour two level i-1
                             sites, so if the site we are currently inspecting has already been added to the level i
                             list that means it is a degenerate site.  What we do then is remove the reference to it we
                             already have and add it to a seperate degenerate sites i(d) list which will be placed before
                             the non-degenerate i list.
                             */
                            if ( current_level_got_sites.count( inspect_site )>0 ) {
                                current_level_list.remove( inspect_site );
                                current_level_degenerate_list.push_back( inspect_site );
                            } else {
                                current_level_list.push_back( inspect_site );
                                current_level_got_sites.insert( inspect_site );
                            }
                        } else {
                            current_level_rejected_sites.insert( inspect_site );
                        }
                    }
                }
            }
        }
        
        /*
         add all the sites identified as distance i sites to the cumulative list of accepted sites.  Then add the level i
         list into the decoder look-up table and update counters
         */
        if (!(current_level_got_sites.empty())) {
            got_sites.insert(current_level_got_sites.begin(), current_level_got_sites.end());
            
            if ( !(current_level_degenerate_list.size()==0) ) {
                decoder_lookup_table.insert( std::make_pair( level, current_level_degenerate_list ) );
                
                std::cout << "LEVEL " << level << " -- ADDED " << current_level_degenerate_list.size() << " SITES.\n";
                level++;
            }
            
            if ( !(current_level_list.size()==0) ) {
                decoder_lookup_table.insert( std::make_pair( level, current_level_list ) );
                
                std::cout << "LEVEL " << level << " -- ADDED " << current_level_list.size() << " SITES.\n";
                level++;
            }
        }
        
        /*
         record all the new sites found on this search as the base for the next level search
         */
        sites_to_search_around.clear();
        sites_to_search_around.insert(current_level_got_sites.begin(), current_level_got_sites.end());
        
        /*
         stop when the level i search has not identified any new sites
         */
    } while ( (!(current_level_got_sites.empty())) );
    
    std::cout << "FINISHED BUILDING LOOKUP TABLE, IN TOTAL GOT " << got_sites.size() << " SITES OUT OF " << sys_size*sys_size << " SITES.\n \n";
}

/*
 ----------------------------------------------------------
 methods to handle the combination of two overlapping boxes
 ----------------------------------------------------------
 */

//  handle the combination of two boxes if they overlap while running the decoder
bool CCSystem::handleBoxCombinations( int box_id_, int other_box_id_, std::set<std::pair<int,int>>& temp_particle_pos_ ) {
    
    // combine the specs (charge and boundaries) of the boxes, particles are not moved yet
    if ( !decoder_boxes.at(box_id_).combineBoxes(decoder_boxes.at(other_box_id_),sys_size,sys_size) ) {
        // if ( fail_fast ) { return false; }
    }
    
    // deal with the refugee particles from the redundant box
    for ( auto refugee : *decoder_boxes.at(other_box_id_).getContainedParticles() ) {
        
        // store that this particle must be moved to its new box
        temp_particle_pos_.insert(refugee);
        
        // change the box id of each of the particles to point to its new box
        stabilisers[refugee.first][refugee.second].setBoxNumber(box_id_);
    }
    
    return true;
}

/*
 ---------------
 run the decoder
 ---------------
 */

//  call to run the decoder, by allowing boxClusters to return false allows to build in fail fast
bool CCSystem::runDecoder() {
    if ( !boxClusters() ) {
        return false;
    } else {
        return testDecoderSuccess();
    }
}

//  method to group all the defects into clusters inside neutral boxes
bool CCSystem::boxClusters() {
    
    decoder_boxes.clear();
    number_charged_boxes = 0;
    
    // std::cout << "BEGINNING TO BOX CLUSTERS...\n";
    
    // STEP 1
    
    // put syndromes each in their own box
    /*
     std::cout << "LEVEL 0 --\n";
     std::cout << "BOX #" << '\t' << '\t' << "CHARGE" << '\t' << '\t' << "POS (x,y)\n";
     std::cout << "----------------------------------------\n";
     */
    
    for ( auto anyon: anyon_positions ) {
        
        CCDecoderBox new_box( anyon.first, anyon.second, stabilisers[anyon.first][anyon.second].color() );
        // std::cout << number_charged_boxes << '\t' << '\t' << "(" << new_box.getLowerLeft().first << "," << new_box.getLowerLeft().second << ")" << '\n';
        
        decoder_boxes.insert( std::make_pair(number_charged_boxes,new_box) );
        stabilisers[anyon.first][anyon.second].setBoxNumber(number_charged_boxes);
        
        number_charged_boxes++;
    }
    
    // do a level 1 search and combine any boxes which overlap
    std::set<int> redundant_boxes;
    std::set<std::pair<int,int>> particles_to_add_to_current_box;
    std::vector<std::pair<int,int>> current_neighbours;
    
    unsigned rg_level_counter = 1;
    number_charged_boxes = static_cast<int>(decoder_boxes.size());
    while ( (number_charged_boxes>0) && (rg_level_counter<(decoder_lookup_table.size())) ) {
        
        /*
         std::cout << "\n \n";
         std::cout << "LEVEL " << rg_level_counter << " --\n";
         std::cout << "THERE ARE " << number_charged_boxes << " CHARGED BOXES REMAINING.\n";
         std::cout << '\n';
         std::cout << "BOX #" << '\t' << '\t' << "CHARGE" << '\t' << "LOWER LEFT" << '\t' << "UPPERR RIGHT\n";
         std::cout << "--------------------------------------------------------\n";
         */
        
        redundant_boxes.clear();
        
        // iterate over all the non-redundant, charged boxes
        for ( auto& box : decoder_boxes ) {
            if ( (!(box.second.isRedundant())) && (!(box.second.isNeutral())) ) {
                
                // iterate over all the particles in the current box
                for ( auto particle : *box.second.getContainedParticles() ) {
                    
                    // iterate over all the level i neighbours of the current particle
                    current_neighbours = getLeveliNeighbours(rg_level_counter,particle.first,particle.second);
                    for ( auto neighbour : current_neighbours ) {
                        
                        // if we find another flipped stabiliser not in the same box
                        if ( stabilisers[neighbour.first][neighbour.second].getOccupation() && !(stabilisers[neighbour.first][neighbour.second].getBoxNumber()==box.first) ) {
                            
                            // absorb the other box if it is charged
                            int other_box_number = stabilisers[neighbour.first][neighbour.second].getBoxNumber();
                            if ( !(decoder_boxes.at(other_box_number).isNeutral()) ) {
                                
                                redundant_boxes.insert(other_box_number);
                                decoder_boxes.at(other_box_number).makeRedundant();
                                
                                // std::cout << "BOX " << box.first << " ABSORBED BOX " << other_box_number << '\n';
                                
                                if ( !handleBoxCombinations(box.first,other_box_number,particles_to_add_to_current_box) ) {
                                    // if ( fail_fast) { return false; }
                                }
                            }
                        }
                    }
                }
                
                // repeat for all the particles that are new to the box, if the box is still charged
                for ( auto new_particle : particles_to_add_to_current_box ) {
                    
                    // iterate over all the level i neighbours of the current particle
                    current_neighbours = getLeveliNeighbours(rg_level_counter,new_particle.first,new_particle.second);
                    for ( auto new_neighbour : current_neighbours ) {
                        
                        // if we find another flipped stabiliser not in the same box
                        if ( stabilisers[new_neighbour.first][new_neighbour.second].getOccupation() && !(stabilisers[new_neighbour.first][new_neighbour.second].getBoxNumber()==box.first) ) {
                            
                            // absorb the other box if it is not neutral from a lower level
                            int other_box_number = stabilisers[new_neighbour.first][new_neighbour.second].getBoxNumber();
                            if ( !(decoder_boxes.at(other_box_number).isNeutral()) ) {
                                
                                redundant_boxes.insert(other_box_number);
                                decoder_boxes.at(other_box_number).makeRedundant();
                                
                                // std::cout << "BOX " << box.first << " ABSORBED BOX " << other_box_number << '\n';
                                
                                if ( !handleBoxCombinations(box.first,other_box_number,particles_to_add_to_current_box) ) {
                                    // if (fail_fast) { return false; }
                                }
                            }
                        }
                    }
                }
                
                // add the new particles into the box
                box.second.addNewParticlePositions(particles_to_add_to_current_box);
                particles_to_add_to_current_box.clear();
            }
        }
        
        // clear redundant boxes from the boxes map
        for ( auto box_to_clear : redundant_boxes ) {
            decoder_boxes.erase(box_to_clear);
            number_charged_boxes--;
        }
        
        // iterate again over all the remaining boxes and check which are now neutral
        for ( auto& box : decoder_boxes ) {
            
            /*
             std::cout << box.first << '\t' << '\t' << box.second.hasOddParity()
             << '\t' << '\t' << "(" << box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << ")"
             << '\t' << '\t' << "(" << box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ")" << '\n';
             std::cout << "--------------------------------------------------------\n";
             for ( auto particle : *box.second.getContainedParticles() ) {
             std::cout << '\t' << '\t' << "(" << particle.first << "," << particle.second << ")\t in " <<
             stabilisers[particle.first][particle.second].getBoxNumber() << '\n';
             }
             std::cout << "--------------------------------------------------------\n";
             */
            
            bool test = box.second.isNewlyNeutral();
            if ( (test) && (box.second.isColorless()) ) {
                number_charged_boxes--;
            }
        }
        
        // go to the next RG level
        rg_level_counter++;
    }
    // std::cout << "ENDING BOXING CLUSTERS..." << '\n';
    
    return true;
}

/*
 --------------------------------------------------------------------------------------
 use the boxes to estimate the logical state and compare this to the real logical state
 --------------------------------------------------------------------------------------
 */

//  compare decoder's guess of log state to real log state
bool CCSystem::testDecoderSuccess() {
    
    measureLogicalState();
    auto infered_state = guessLogicalStateFromDecoder();
    
    // std::cout << "MEASURED " << logical_state.first << '\t' << logical_state.second << '\n';
    // std::cout << "INFERRED " << infered_state.first << '\t' << infered_state.second << '\n';
    
    bool test = (std::get<0>(logical_state)==std::get<0>(infered_state));
    test = test && (std::get<1>(logical_state)==std::get<1>(infered_state));
    test = test && (std::get<2>(logical_state)==std::get<2>(infered_state));
    test = test && (std::get<3>(logical_state)==std::get<3>(infered_state));
    return test;
}

std::tuple<bool,bool,bool,bool> CCSystem::guessLogicalStateFromDecoder() {
    std::tuple<bool,bool,bool,bool> guessed_logical_state;
    std::get<0>(guessed_logical_state) = false;
    std::get<1>(guessed_logical_state) = false;
    std::get<2>(guessed_logical_state) = false;
    std::get<3>(guessed_logical_state) = false;
    int total_charge_crossing;
    
    // std::cout << '\n' << "CALCULATING FLUX ACROSS LOGICAL OPERATORS -----\n";
    for ( auto box : decoder_boxes ) {
        
        //  logical_state[0]
        if ( std::get<0>(box.second.mightAffectLog()) ) {
            /*
             std::cout << "ADDING HORIZONTAL FLUX FROM BOX: " << box.first << " --- " <<
             "(" << box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << ")"
             << " -> " << "(" << box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ")" << '\n';
             */
            total_charge_crossing = 0;
            for ( auto particle : *box.second.getContainedParticles() ) {
                if ( (particle.second>0) && (particle.second<(box.second.getUpperRight().second+1)) ) {
                    total_charge_crossing = box.second.fuseColors(total_charge_crossing, stabilisers[particle.first][particle.second].color());
                }
            }
            if ( (total_charge_crossing==1) || (total_charge_crossing==2) ) {
                std::get<0>(guessed_logical_state) = std::get<0>(guessed_logical_state)^true;
            }
        }
        
        //  logical_state[1]
        if ( std::get<1>(box.second.mightAffectLog()) ) {
            /*
             std::cout << "ADDING HORIZONTAL FLUX FROM BOX: " << box.first << " --- " <<
             "(" << box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << ")"
             << " -> " << "(" << box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ")" << '\n';
             */
            total_charge_crossing = 0;
            for ( auto particle : *box.second.getContainedParticles() ) {
                if ( (particle.first<(sys_size-1)) && (particle.first>(box.second.getLowerLeft().first-1)) ) {
                    total_charge_crossing = box.second.fuseColors(total_charge_crossing, stabilisers[particle.first][particle.second].color());
                }
            }
            if ( (total_charge_crossing==1) || (total_charge_crossing==2) ) {
                std::get<1>(guessed_logical_state) = std::get<1>(guessed_logical_state)^true;
            }
        }
        
        //  logical_state[2]
        if ( std::get<2>(box.second.mightAffectLog()) ) {
            /*
             std::cout << "ADDING HORIZONTAL FLUX FROM BOX: " << box.first << " --- " <<
             "(" << box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << ")"
             << " -> " << "(" << box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ")" << '\n';
             */
            total_charge_crossing = 0;
            for ( auto particle : *box.second.getContainedParticles() ) {
                if ( (particle.second<(sys_size-1)) && (particle.second>(box.second.getLowerLeft().second-1)) ) {
                    total_charge_crossing = box.second.fuseColors(total_charge_crossing, stabilisers[particle.first][particle.second].color());
                }
            }
            if ( (total_charge_crossing==2) || (total_charge_crossing==3) ) {
                std::get<2>(guessed_logical_state) = std::get<2>(guessed_logical_state)^true;
            }
        }
        
        //  logical_state[3]
        if ( std::get<3>(box.second.mightAffectLog()) ) {
            /*
             std::cout << "ADDING HORIZONTAL FLUX FROM BOX: " << box.first << " --- " <<
             "(" << box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << ")"
             << " -> " << "(" << box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ")" << '\n';
             */
            total_charge_crossing = 0;
            for ( auto particle : *box.second.getContainedParticles() ) {
                if ( (particle.first>0) && (particle.first<(box.second.getUpperRight().first+1)) ) {
                    total_charge_crossing = box.second.fuseColors(total_charge_crossing, stabilisers[particle.first][particle.second].color());
                }
            }
            if ( (total_charge_crossing==2) || (total_charge_crossing==3) ) {
                std::get<3>(guessed_logical_state) = std::get<3>(guessed_logical_state)^true;
            }
        }
    }
    
    return guessed_logical_state;
}

/*
 --------------
 test functions
 --------------
 */

//  check for things that might have gone wrong in the boxing of particles
void CCSystem::testBoxParticleMatching() {
    
    std::cout << '\n' << "TESTING BOX PARTICLE MATCHING --- \n" << '\n';
    
    std::map<std::pair<int,int>,int> particlesAlreadySeen;
    
    // iterate over all the boxes
    for ( auto box : decoder_boxes ) {
        
        // iterate over all the particles in the Box
        for ( auto particle : *box.second.getContainedParticles() ) {
            
            // check that this particle is not included in a box we have already looked at
            if ( particlesAlreadySeen.count(particle)>0 ) {
                std::cout << stabilisers[particle.first][particle.second].color() << '\t' << "@" <<
                '\t' << "(" << particle.first << "," << particle.second << ") SEEN IN " <<
                particlesAlreadySeen.at(particle) << " AND " << box.first << '\n';
            } else {
                particlesAlreadySeen.insert( std::make_pair( particle, box.first ) );
            }
            
            // check that the particle identifies as being in the right box
            if ( !(stabilisers[particle.first][particle.second].getBoxNumber()==box.first) ) {
                std::cout << stabilisers[particle.first][particle.second].color() << '\t' << "@" <<
                '\t' << "(" << particle.first << "," << particle.second << ") SAYS IT IS IN " <<
                stabilisers[particle.first][particle.second].getBoxNumber() << " BUT SEEN IN " << box.first << '\n';
            }
            
            // check that the particle is within physical limits of the box
            if ( !(box.second.coordInsideBox(particle)) ) {
                std::cout << stabilisers[particle.first][particle.second].color() << '\t' << "@" <<
                '\t' << "(" << particle.first << "," << particle.second << ") RECORDED IN BOX " <<
                box.first << " BUT OUTSIDE LIMITS OF BOX -- (" <<
                box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << ") -> (" <<
                box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ")\n";
            }
        }
    }
}
