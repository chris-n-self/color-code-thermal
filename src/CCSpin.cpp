//
//  CCSpin.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSpin.h"

CCSpin::CCSpin() {
    state = false;
    xcoord = 0;
    ycoord = 0;
    id_num = 0;
    rate_key = 0;
}

CCSpin::CCSpin( int x_, int y_, int id_num_ ) {
    state = false;
    xcoord = x_;
    ycoord = y_;
    id_num = id_num_;
    rate_key = 0;
}