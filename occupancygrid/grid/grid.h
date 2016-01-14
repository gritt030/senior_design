
#ifndef OG_GRID_
#define OG_GRID_

#include <iostream> //TODO:remove

class Grid {
  public:
    static const int GRID_SIZE = 1611;   //size of one side of the grid (odd) 61 for good data structure
    static const char MAX_VALUE = 127;  //maximum value we can have in a grid square
    char *map;                          //occupancy grid for this node
    
    //neighbors in linked list
    Grid *N=nullptr,    //y increasing
         *S=nullptr,    //y decreasing
         *E=nullptr,    //x increasing
         *W=nullptr;    //x decreasing
    
    //TODO: Not every node needs 4 neighbors if we always start at (0,0) and
            //move around from there. Only root needs 4 neighbors, all other
            //nodes need 2 or 3 neighbors at the most.
    
    //constructor
    Grid();
    
    //set and get values in grid
    void setValue(int x, int y, char value);
    void changeValue(int x, int y, char value);
    char getValue(int x, int y);
};

#endif