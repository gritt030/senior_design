
#ifndef HOUGHGRID_H
#define HOUGHGRID_H

#include <iostream>
#include <cmath>
#include "grid.h"

class HoughGrid
{
  public:
    static const int GRID_SIZE = (int)((float)Grid::GRID_SIZE * 1.414213562f) + 1;   //size of one side of the grid
    static const int CENTER = Grid::GRID_SIZE/2;
    static const int ADDITION = GRID_SIZE/2;
    static const unsigned short MAX_VALUE = 65535;  //maximum value we can have in a grid square
    
    unsigned short *map;  //occupancy grid for this node
    
    double D_THETA;
    
    //theta used for cardinal directions
    double X_Cardinal=-1.0, Y_Cardinal=-1.0;
    
    //constructor/destructor
    HoughGrid();
    ~HoughGrid();
    
    //set and get values in grid
    void setValue(int x, int y, unsigned short value);
    void changeValue(int x, int y, unsigned short value);
    unsigned short getValue(int x, int y);
    
    void addHoughPoint(int x, int y);
    void removeHoughPoint(int x, int y);
    void addHoughPointWeighted(int x, int y, unsigned short weight);
};

#endif // HOUGHGRID_H
