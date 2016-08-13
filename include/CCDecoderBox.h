//
//  CCDecoderBox.h
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#ifndef __ColorCodeThermal__CCDecoderBox__
#define __ColorCodeThermal__CCDecoderBox__

class CCDecoderBox {
private:
    //  total net color of all particles in box
    int total_net_color;
    
    //  the dimensions of the box -- box is fully rectangular
	std::pair<int,int> upper_right;
	std::pair<int,int> lower_left;
    
    //  list of the particles in the box
	std::set<std::pair<int,int>> contained_particles;
    
    //  flags used for garbage collection by decoder
	bool has_been_absorbed;
	bool has_been_neutralised;
    
	//  flags for color flux affecting the log ops
    std::tuple<bool,bool,bool,bool> might_affect_log;
    
public:
    //  constructor
	CCDecoderBox( int at_x_, int at_y_, int color_ );
    
    //  color combination method
    static int fuseColors( int color_1_, int color_2_ );
    
    //  access methods
    bool isColorless() { return (total_net_color == 0); };
    int getTotalColor() { return total_net_color; }
    //
    std::pair<int,int> getUpperRight() { return upper_right; }
    std::pair<int,int> getLowerLeft() { return lower_left; }
    std::set<std::pair<int,int>>* getContainedParticles() { return &contained_particles; }
    bool isRedundant() { return has_been_absorbed; }
    bool isNeutral() { return has_been_neutralised; }
    std::tuple<bool,bool,bool,bool> mightAffectLog() { return might_affect_log; }
    
    bool isNewlyNeutral(); //  ask if this box became neutral at this RG level
    bool combineBoxes( CCDecoderBox& other_box_, int repeat_x_, int repeat_y_ ); //  absorb another box into this box
    void makeRedundant(); //  signal this box has been absorbed by another
    bool coordInsideBox( std::pair<int,int> coord ); //  ask if a given coord is inside the box
    void addNewParticlePositions( std::set<std::pair<int,int>>& newParticles ); //  add a list of particles to this box
    
};

#endif /* defined(__ColorCodeThermal__CCDecoderBox__) */
