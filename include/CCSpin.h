//
//  CCSpin.h
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#ifndef __ColorCodeThermal__CCSpin__
#define __ColorCodeThermal__CCSpin__

class CCSpin {
private:
    bool state; //  whether or not the spin has suffered a bit-flip error

    // coordinates
    int xcoord;
    int ycoord;
    int id_num;
    
    // thermal properties
    int rate_key;

public:
    CCSpin();
    CCSpin( int x_, int y_, int id_num_ );
    bool getState() { return state; }
    void setState( bool new_state_ ) { state = new_state_; }
    void flipState() { state = !state; }
    int x() { return xcoord; }
    int y() { return ycoord; }
    int idNum() { return id_num; }
    
    // thermal functions
    int getRateKey() { return rate_key; }
    void setRateKey( int new_key_ ) { rate_key = new_key_; }
};

#endif /* defined(__ColorCodeThermal__CCSpin__) */
