//
//  CCStabiliser.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCStabiliser.h"

CCStabiliser::CCStabiliser() {
    occupied = false;
    xcoord = 0;
    ycoord = 0;
    color_label = 0;
    box_number = 0;
}

CCStabiliser::CCStabiliser( int x_, int y_, int color_ ) {
    occupied = false;
    xcoord = x_;
    ycoord = y_;
    color_label = color_;
    box_number = 0;
}