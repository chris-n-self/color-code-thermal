//
//  asymptote3d.h
//  ToricCodeThermal
//
//  Created by Chris Self on 04/09/2014.
//  Copyright (c) 2014 Chris Self. All rights reserved.
//

#ifndef __asymptote3d__
#define __asymptote3d__

struct Cmp {
    bool operator ()(const std::pair<double,std::string> &a, const std::pair<double,std::string> &b) {
        return a.first < b.first;
    }
};

class AsymptoteCommands {
private:
    std::set<std::pair<double,std::string>,Cmp> list_of_commands;
public:
    AsymptoteCommands();
    double depth( int x_, int y_, int z_ );
    void submit( std::pair<double,std::string> command_ );
    void output( std::ofstream& dout );
};

#endif /* defined(__asymptote3d__) */
