//
//  asymptote3d.cpp
//  ToricCodeThermal
//
//  Created by Chris Self on 04/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "asymptote3d.h"
double rad( double degrees_ );
std::pair<int, int> triProj( int x_, int y_, int z_ );

using namespace std;

double vertical_tilt_angle = 40.0;
double horizontal_pan_angle = -30.0;

double rad( double degrees_ ) {
    return degrees_*(PI/180.0);
}

pair<int, int> triProj( int x_, int y_, int z_ ) {
    double x_proj = cos( rad(horizontal_pan_angle) )*x_ + sin( rad(horizontal_pan_angle) )*z_;
	double y_proj = cos( rad(vertical_tilt_angle) )*y_ - sin( rad(vertical_tilt_angle) )*( -sin( rad(horizontal_pan_angle) )*x_ + cos( rad(horizontal_pan_angle) )*z_ );
	return make_pair( x_proj, y_proj );
}

double AsymptoteCommands::depth( int x_, int y_, int z_ ) {
    double z_proj = sin( rad(vertical_tilt_angle) )*y_ + cos( rad(horizontal_pan_angle) )*( -sin( rad(horizontal_pan_angle) )*x_ + cos( rad(horizontal_pan_angle) )*z_ );
    return z_proj;
}

void AsymptoteCommands::submit( std::pair<double,std::string> command_ ) {
    list_of_commands.insert( command_ );
}

void AsymptoteCommands::output( std::ofstream &dout ) {
    for ( auto command : list_of_commands ) {
        dout << command.second;
    }
}