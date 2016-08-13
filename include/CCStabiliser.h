//
//  CCStabiliser.h
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#ifndef __ColorCodeThermal__CCStabiliser__
#define __ColorCodeThermal__CCStabiliser__

class CCStabiliser {
private:
	bool occupied;
	int xcoord;
    int ycoord;

    //  color
    int color_label;
    
    // decoder information
    int box_number;
    
public:
	CCStabiliser();
	CCStabiliser( int x_, int y_, int color_ ); //  constructor with options
    int x() { return xcoord; }
    int y() { return ycoord; }
    int color() { return color_label; }
    bool getOccupation() { return occupied; }
    void setOccupation( bool state_ ) { occupied = state_; }
    void flipOccupation() { occupied = !occupied; }
    
    // decoder methods
    int getBoxNumber() { return box_number; }
    void setBoxNumber( int new_box_number_ ) { box_number = new_box_number_; }
    
};

#endif /* defined(__ColorCodeThermal__CCStabiliser__) */
