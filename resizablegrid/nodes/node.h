
#ifndef RGRID_NODE_
#define RGRID_NODE_

#include <iostream> //TODO:remove

class Node {
  public:
    static const int GRID_SIZE = 161;    //size of one side of the grid (odd) 61 for good data structure
    char *map;                          //occupancy grid for this node
    
    //neighbors in linked list
    Node *N=nullptr,    //y increasing
         *S=nullptr,    //y decreasing
         *E=nullptr,    //x increasing
         *W=nullptr;    //x decreasing
    
    //TODO: Not every node needs 4 neighbors if we always start at (0,0) and
            //move around from there. Only root needs 4 neighbors, all other
            //nodes need 2 or 3 neighbors at the most.
    
    //constructor
    Node();
    
    //set and get values in grid
    void setValue(int x, int y, char value);
    void changeValue(int x, int y, char value);
    char getValue(int x, int y);
};

#endif