
#ifndef HOUGHGRID_H
#define HOUGHGRID_H

#include <iostream>
#include <cmath>
#include "grid.h"

class HoughGrid
{
  public:
    static const int GRID_SIZE = (int)((float)Grid::GRID_SIZE * 1.414213562f) + Grid::GRID_SIZE + 1;   //size of one side of the grid
    static const unsigned short MAX_VALUE = 65535;  //maximum value we can have in a grid square
    //static const int DISTANCE = 2;  //separation of peaks
    static const int NUM_PEAKS = 300;   //number of peaks to find
    unsigned short *map;                          //occupancy grid for this node
    
    double D_THETA;
    
    //constructor/destructor
    HoughGrid();
    ~HoughGrid();
    
    //set and get values in grid
    void setValue(int x, int y, unsigned short value);
    void changeValue(int x, int y, unsigned short value);
    unsigned short getValue(int x, int y);
    
    void addHoughPoint(int x, int y);
    void addHoughPointWeighted(int x, int y, unsigned short weight);
    
    void findMaxima();
    void maxima1();
    void maxima2();
    int submaxima1(unsigned short* valArr);
};

#endif // HOUGHGRID_H
