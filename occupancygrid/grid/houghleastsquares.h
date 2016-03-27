#ifndef HOUGHLEASTSQUARES_H_
#define HOUGHLEASTSQUARES_H_

#include <iostream>
#include <cmath>
#include "houghgrid.h"

class HoughLeastSquares
{
public:
  //size of boxes to use for final least squares
  static const int LS_RADIUS = 37;
  static const int LS_ANGLE = 37;
  //theta used for cardinal directions
  double X_Cardinal=-1.0, Y_Cardinal=-1.0;
  static const int NUM_PEAKS = 2000;   //number of peaks to find
//   static const unsigned short THRESHOLD = 0;
    
  
  HoughLeastSquares(HoughGrid* grid);
  ~HoughLeastSquares();
  void performLeastSquares();
  void leastSquare(int radSize, int angSize);
  void mergePeaks();
  void getClosestPeak(int* buf);
  int getDelta(int t1, int r1, int t2, int r2);
  
  HoughGrid* houghGrid;
  HoughGrid* lsqrGrid = nullptr;
  
  void findMaxima();
  void maxima0();
  void submax0(int i, int j, int max, int min, int version, HoughGrid* newGrid);
  void maxima1();
  void maxima2();
  int submaxima1(unsigned short* valArr);
  void detectHist();
  
  
};

#endif // HOUGHLEASTSQUARES_H
