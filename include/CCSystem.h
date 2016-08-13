//
//  CCSystem.h
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "CCSpin.h"
#include "CCStabiliser.h"
#include "CCDecoderBox.h"

#ifndef __ColorCodeThermal__CCSystem__
#define __ColorCodeThermal__CCSystem__

class CCSystem {
private:
	/*
     member variables
     */
    // spins, plaquettes and anyons
    int sys_size;
    std::vector<std::vector<CCStabiliser>> stabilisers;
    std::vector<std::vector<std::map<int,CCSpin>>> spins;
    std::set<std::pair<int,int>> anyon_positions;
    
    // logical state
    std::tuple<bool,bool,bool,bool> logical_state;
    
    // decoder resources
    std::map<int,CCDecoderBox> decoder_boxes;
    int number_charged_boxes;
    std::map<int,std::list<std::pair<int,int>>> decoder_lookup_table;
    
    // noise
    //
    // random number generator
    static MTRand_closed rand_gen;
    //
    // thermal
    double simulation_time;
    double total_rate;
    std::map<int,int> rates_class_counts;
    
    /*
     functions
     */
    // lattice geometry
    std::list<std::pair<int,int>> getNeighbours( int from_x_, int from_y_ );
    std::vector<std::pair<int,int>> getLeveliNeighbours( int level_i_, int from_x_, int from_y_ );
    
    // internal decoder functions
    std::tuple<bool,bool,bool,bool> guessLogicalStateFromDecoder();
    bool testDecoderSuccess();
    bool boxClusters();
    bool handleBoxCombinations( int box_id_, int other_box_id_, std::set<std::pair<int,int>>& temp_particle_pos_ );
    
    // interal thermal noise functions
    void setThermalRates();
    static std::map<int,double> createRatesMap();
    void incrementTime();
    void thermalSpinFlip();
    void handleStabiliserChange( int x_, int y_ );
    void updateLocalRates( int x_, int y_, int id_ );
    void updateSpinRate( int x_, int y_, int id_ );
    
public:
    /*
     member variables
     */
    // thermal resources
    static const double beta;
    std::map<int,double> thermal_rates;
    
    /*
     functions
     */
    CCSystem( int L_ );
    int L() { return sys_size; }
    void buildDecoderLookupTable();
    
    // system processes
    void evaluateSyndromes();
    void findAnyons();
    void measureLogicalState();
    
    // decoder functions
    bool runDecoder();
    //
    // test functions
    void testBoxParticleMatching();
    
    // noise
    //
    // generic
    void resetSystem();
    void flipSpin( int at_x_, int at_y_, int at_id_ );
    //
    // iid
    void sufferIIDNoise( double error_rate_ );
    double calcIIDSuccessRate( double error_rate_, long num_trials_ );
    //
    // thermal
    void thermalEvolution( double end_time_ );
    double calcThermalSuccessRate( double end_time_, long int num_trials_ );
    double calcThermalSuccessRateFailFast( double end_time_, long int num_trials_ );
    std::pair<double,double> estimateThermalCoherenceTime( long int num_trials_ );
    
    // drawing
    std::pair<int,int> convertToDrawCoord( std::pair<int,int> coord_ );
    void drawAll( std::ofstream& dout );
    void drawEmptyLattice( std::ofstream& dout );
    void drawQubits( std::ofstream& dout );
    void drawQubitsRates( std::ofstream& dout );
    void drawAnyons( std::ofstream& dout );
    void drawErrors( std::ofstream& dout );
    void drawBoxes( std::ofstream& dout );
    void drawAnimationHeader( std::ofstream& dout );
    void drawAnimationFrame( std::ofstream& dout, int flipped_x_, int flipped_y_, int flipped_id_ );
    void drawAnimationFooter( std::ofstream& dout );
    void drawLookupTable( int centre_x_, int centre_y_, std::ofstream& dout );
    
};

#endif /* defined(__ColorCodeThermal__CCSystem__) */
