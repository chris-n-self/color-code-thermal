//
//  CCSystem.cpp
//  ColorCodeThermal
//
//  Created by Chris Self on 15/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#include "libraries.h"
#include "CCSystem.h"

void CCSystem::drawAll( std::ofstream &dout ) {
    drawEmptyLattice(dout);
    drawErrors(dout);
    drawBoxes(dout);
    drawAnyons(dout);
}

void CCSystem::drawEmptyLattice( std::ofstream &dout ) {
    dout << std::boolalpha << "import ColorCodeHeader;\n" << '\n'
    << "setL( " << sys_size << " );" << '\n'
    << "drawEmptyLattice();"  << '\n';
    /*
    for ( auto stabil_row : stabilisers ) {
        for ( auto stabil : stabil_row ) {
            dout << std::boolalpha << "placeHexagon( " << stabil.x() << ", " << stabil.y() << ", " << stabil.color() << " );\n";
            dout << std::boolalpha << "labelCoord( " << stabil.x() << ", " << stabil.y() << ", " << "\"" << stabil.x() << ", " << stabil.y() << "\"" << " );\n";
        }
    }
    for ( auto stabil_row : stabilisers ) {
        for ( auto stabil : stabil_row ) {
            dout << std::boolalpha << "placeQubit( " << stabil.x() << ", " << stabil.y() << ", 1 );\n";
            dout << std::boolalpha << "placeQubit( " << stabil.x() << ", " << stabil.y() << ", 2 );\n";
        }
    }
     */
}

void CCSystem::drawAnyons( std::ofstream &dout ) {
    for ( auto anyon : anyon_positions ) {
        dout << std::boolalpha << "placeAnyon( " << anyon.first << ", " << anyon.second << ", " << stabilisers[anyon.first][anyon.second].color() << " );\n";
    }
    dout << '\n';
}

void CCSystem::drawErrors( std::ofstream &dout ) {
    for ( auto spin_row : spins ) {
        for ( auto spin_plaq : spin_row ) {
            for ( auto spin : spin_plaq ) {
                if ( spin.second.getState() ) {
                    dout << std::boolalpha << "placeError( " << spin.second.x() << ", " << spin.second.y() << ", " << spin.second.idNum() << " );\n";
                }
            }
        }
    }
    dout << '\n';
}

void CCSystem::drawBoxes( std::ofstream &dout ) {
    for ( auto box : decoder_boxes ) {
        dout << std::boolalpha << "drawBox( (" << box.second.getLowerLeft().first << "," << box.second.getLowerLeft().second << "), (" << box.second.getUpperRight().first << "," << box.second.getUpperRight().second << ") );\n";
        for ( auto particle : *box.second.getContainedParticles() ) {
            dout << std::boolalpha << "labelAnyonBox( " << particle.first << ", " << particle.second << ", " << "\"" << stabilisers[particle.first][particle.second].getBoxNumber() << "\"" << " );\n";
        }
    }
    dout << '\n';
}

void CCSystem::drawLookupTable( int centre_x_, int centre_y_, std::ofstream& dout ) {
    for ( int i=1; i<(static_cast<int>(decoder_lookup_table.size())+1); ++i ) {
        for ( auto stabil : getLeveliNeighbours(i,centre_x_,centre_y_) ) {
            dout << std::boolalpha << "labelCoord( " << stabil.first << ", " << stabil.second << ", " << "\"" << i << "\"" << " );\n";
        }
    }
}
