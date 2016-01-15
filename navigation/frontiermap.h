
#ifndef FRONTIERMAP_H_
#define FRONTIERMAP_H_

#include "./../pngwriter/png_writer.h"
#include "./../occupancygrid/grid/grid.h"

class FrontierMap
{
public:
  //"length" of quadcopter on actual map
  static const int SCALE = 9;
  //size of grid to use
  static const int GRID_SIZE = Grid::GRID_SIZE / SCALE;
  //used to correct coordinates when setting values in grid
  static const int BOUNDARY = GRID_SIZE / 2;
  //actual size of grid used
  static const int ACTUAL_SIZE = GRID_SIZE*GRID_SIZE;
  
  // open = 11, closed = 00, frontier = 01
  
  //constructor/destructor
  FrontierMap();
  ~FrontierMap();
  
  //output map as image
  void sendToImage(char* filename);
  
  //manipulate grid
  void openSquare(int x, int y);
  void occupySquare(int x, int y);
  void closeSquare(int x, int y);
  void frontierSquare(int x, int y);
  int getSquare(int x, int y);
  
private:
  char* frontierGrid;
  
  //set a pixel in the image
  void setImagePixel(PngWriter* w, int x, int y, int value);

};

#endif // FRONTIERMAP_H
