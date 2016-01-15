
#ifndef FRONTIERMAP_H_
#define FRONTIERMAP_H_

#include "./../pngwriter/png_writer.h"
#include "./../occupancygrid/grid/grid.h"

class FrontierMap
{
public:
  static const int GRID_SIZE = Grid::GRID_SIZE / 9;
  
  //constructor/destructor
  FrontierMap();
  ~FrontierMap();
  
  //output map as image
  void sendToImage(char* filename);
  
private:
  char* frontierGrid;
  
  //set a pixel in the image
  void setImagePixel(PngWriter* w, int x, int y, char value);

};

#endif // FRONTIERMAP_H
