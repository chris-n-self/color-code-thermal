//
//  CCDecoderBox.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCDecoderBox.h"

int furthestPointToRight ( std::pair<int,int> pairA, std::pair<int,int> pairB, int repeatLength );
int lengthOf( int left, int right, int repeatLength );

CCDecoderBox::CCDecoderBox( int at_x_, int at_y_, int color_ ) {
    total_net_color = color_;
    
    upper_right = std::make_pair( at_x_, at_y_ );
    lower_left = upper_right;
    
    contained_particles.insert(upper_right);
    
    has_been_neutralised = false;
    has_been_absorbed = false;
    
    might_affect_log = std::make_tuple(false,false,false,false);
}

//  ask box if it became neutral during current RG level of decoder
bool CCDecoderBox::isNewlyNeutral() {
    if (has_been_neutralised) {
        return false;
    }
    has_been_neutralised = isColorless();
    return has_been_neutralised;
}

//  get resultant color from fusing two anyons
int CCDecoderBox::fuseColors( int color_1_, int color_2_ ) {

    if ( (color_1_==0) || (color_2_==0) ) { // happens when adding a net charge to a neutral box
        return color_1_+color_2_;
    } else if ( color_1_ == color_2_ ) {
        return 0;
    } else if ( (color_1_+color_2_) == 3 ){
        return 3;
    } else if ( (color_1_+color_2_) == 4 ){
        return 2;
    } else if ( (color_1_+color_2_) == 5 ){
        return 1;
    } else {
        std::cout << "ERROR trying to fuse colors " << color_1_ << " and " << color_2_ << '\n';
        return 0;
    }
}

/*
 ----------------------------------------------------------
 methods to handle the combination of two overlapping boxes
 ----------------------------------------------------------
 */

//  signal this box has been absorbed by another
void CCDecoderBox::makeRedundant() {
    has_been_absorbed = true;
}

//  absorb another box into this, calculate the new edges of the combined box and the new color
//  (this algorithm does not really work if the boxes are ~ L/2 size)
//
bool CCDecoderBox::combineBoxes( CCDecoderBox& other_box_, int repeat_x_, int repeat_y_ ) {
    
    //  Handle the new boundaries of the box
    std::pair<int,int> box_a;
    std::pair<int,int> box_b;
    
    /*
     LEFT-RIGHT
     */
    std::pair<int,int> this_box_left_right = std::make_pair( lower_left.first, upper_right.first );
    std::pair<int,int> other_box_left_right = std::make_pair( other_box_.getLowerLeft().first, other_box_.getUpperRight().first );
    
    //  Box A defined as having its left hand edge at a lower value of the coordinate
    if ( this_box_left_right.first < other_box_left_right.first ) {
        box_a = this_box_left_right;
        box_b = other_box_left_right;
    } else {
        box_a = other_box_left_right;
        box_b = this_box_left_right;
    }
    
    //  do they overlap?
    if ( !(box_a.first > box_b.second) && (box_b.second < box_b.first) ) {
        lower_left.first = box_b.first;
        upper_right.first = furthestPointToRight( box_a, box_b, repeat_x_ );
    }
    else if ( !(box_a.second < box_b.first) || (box_a.second < box_a.first) ) {
        lower_left.first = box_a.first;
        upper_right.first = furthestPointToRight( box_a, box_b, repeat_x_ );
    }
    
    //  if they do not overlap
    else if ( lengthOf( box_a.first, box_b.second, repeat_x_ ) > lengthOf( box_b.first, box_a.second, repeat_x_ ) ) {
        lower_left.first = box_b.first;
        upper_right.first = box_a.second;
    } else {
        lower_left.first = box_a.first;
        upper_right.first = box_b.second;
        
    }
    
    /*
     UP-DOWN
     */
    std::pair<int,int> this_box_up_down = std::make_pair( lower_left.second, upper_right.second );
    std::pair<int,int> other_box_up_down = std::make_pair( other_box_.getLowerLeft().second, other_box_.getUpperRight().second );
    
    if ( this_box_up_down.first < other_box_up_down.first ) {
        box_a = this_box_up_down;
        box_b = other_box_up_down;
    } else {
        box_a = other_box_up_down;
        box_b = this_box_up_down;
    }
    
    //  do they overlap?
    if ( !(box_a.first > box_b.second) && (box_b.second < box_b.first) ) {
        lower_left.second = box_b.first;
        upper_right.second = furthestPointToRight( box_a, box_b, repeat_y_ );
    }
    else if ( !(box_a.second < box_b.first) || (box_a.second < box_a.first) ) {
        lower_left.second = box_a.first;
        upper_right.second = furthestPointToRight( box_a, box_b, repeat_y_ );
    }
    
    //  if they do not overlap
    else if ( lengthOf( box_a.first, box_b.second, repeat_y_ ) > lengthOf( box_b.first, box_a.second, repeat_y_ ) ) {
        lower_left.second = box_b.first;
        upper_right.second = box_a.second;
    } else {
        lower_left.second = box_a.first;
        upper_right.second = box_b.second;
    }
    
    // update color of box by fusing net colors of boxes
    total_net_color = fuseColors( total_net_color, other_box_.getTotalColor() );
    
    //  set flags for if the box crosses a logical operator
    //  logical_state[0]
    if ( (upper_right.second>0) && ( (lower_left.second>upper_right.second) || (lower_left.second==0) ) ) {
        std::get<0>(might_affect_log) = true;
    }
    //  logical_state[1]
    if ( (lower_left.first<(repeat_x_-1)) && ( (lower_left.first>upper_right.first) || (upper_right.first==(repeat_x_-1)) ) ) {
        std::get<1>(might_affect_log) = true;
    }
    //  logical_state[2]
    if ( (lower_left.second<(repeat_y_-1)) && ( (lower_left.second>upper_right.second) || (upper_right.second==(repeat_y_-1)) ) ) {
        std::get<2>(might_affect_log) = true;
    }
    //  logical_state[3]
    if ( (upper_right.first>0) && ( (lower_left.first>upper_right.first) || (lower_left.first==0) ) ) {
        std::get<3>(might_affect_log) = true;
    }
    
    if ( (lengthOf(upper_right.first, lower_left.first, repeat_x_)>(repeat_x_/2)) || (lengthOf(upper_right.second, lower_left.second, repeat_y_)>(repeat_y_/2)) ) {
        return false;
    } else {
        return true;
    }
}


//  ask the box if a given coordinate is inside it
bool CCDecoderBox::coordInsideBox( std::pair<int,int> coord_ ) {
    
    //  test the horizontal dimension
    //  does this box wrap?
    if ( upper_right.first < lower_left.first ) {
        if ( (coord_.first>upper_right.first) && (coord_.first<lower_left.first) ) {
            return false;
        }
    } else {
        if ( (coord_.first>upper_right.first) || (coord_.first<lower_left.first) ) {
            return false;
        }
    }
    
    //  test the vertical dimension
    //  does this box wrap?
    if ( upper_right.second < lower_left.second ) {
        if ( (coord_.second>upper_right.second) && (coord_.second<lower_left.second) ) {
            return false;
        }
    } else {
        if ( (coord_.second>upper_right.second) || (coord_.second<lower_left.second) ) {
            return false;
        }
    }
    
    return true;
}

/*
 ---------------
 other functions
 ---------------
 */

//  add a list of particles to this box, needed separately because have to delay this part of the process while finish iterating over the particles
void CCDecoderBox::addNewParticlePositions( std::set<std::pair<int,int>>& new_particles_ ) {
    for ( auto particle : new_particles_ ) {
        contained_particles.insert(particle);
    }
}

//  function to calculate which interval extends further to the 'right' on a ring
int furthestPointToRight ( std::pair<int,int> pairA, std::pair<int,int> pairB, int repeatLength ) {
    int A = pairA.second;
    int B = pairB.second;
    
    if ( pairA.second < pairA.first ) { A += repeatLength; }
    if ( pairB.second < pairB.first ) {
        B += repeatLength;
        
        /*
         there is a special case where B wraps and A does not but sits across the right-hand end of B, A must also be boosted to get the
         correct answer
         */
        if ( !(pairA.first > pairB.second) ) {
            A += repeatLength;
        }
    }
    
    if ( A>B ) { return pairA.second; }
    else { return pairB.second; }
}
//  function to calculate the distance between two points on a ring
int lengthOf( int left, int right, int repeatLength ) {
    return (repeatLength+(right-left))%repeatLength;
}
