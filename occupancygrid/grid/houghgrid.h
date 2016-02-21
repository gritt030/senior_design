
#ifndef HOUGHGRID_H
#define HOUGHGRID_H

#include <iostream>
#include <cmath>
#include "grid.h"

class HoughGrid
{
  public:
    static const int GRID_SIZE = (int)((float)Grid::GRID_SIZE * 1.414213562f) + 1;// + Grid::GRID_SIZE;   //size of one side of the grid
    static const int CENTER = Grid::GRID_SIZE/2;
    static const int ADDITION = (int)((float)Grid::GRID_SIZE * 0.7071067812);
    static const unsigned short MAX_VALUE = 65535;  //maximum value we can have in a grid square
    //static const int DISTANCE = 2;  //separation of peaks
    static const int NUM_PEAKS = 250;   //number of peaks to find
    
    //size of boxes to use for least squares
    static const int LS_RADIUS = 15;
    static const int LS_ANGLE = 51;//10;
    
    unsigned short *map;                          //occupancy grid for this node
    
    double D_THETA;
    
    //theta used for cardinal directions
    double X_Cardinal, Y_Cardinal;
    
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
    void detectHist();
    void leastSquares();
};

#endif // HOUGHGRID_H
