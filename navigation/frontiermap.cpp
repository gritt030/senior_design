
#include "frontiermap.h"

FrontierMap::FrontierMap() {
  this->frontierGrid = new char[ACTUAL_SIZE]();
}

FrontierMap::~FrontierMap() {
  delete this->frontierGrid;
}



void FrontierMap::openSquare(int x, int y){
  x /= SCALE;
  y /= SCALE;
  x = BOUNDARY + x;
  y = BOUNDARY - y;
  
  int index = x*GRID_SIZE + y;
  
  if (index >= ACTUAL_SIZE) return;
  
  if (this->frontierGrid[index] < 127)
  this->frontierGrid[index] += 1;
}


void FrontierMap::occupySquare(int x, int y){
  x /= SCALE;
  y /= SCALE;
  x = BOUNDARY + x;
  y = BOUNDARY - y;
  
  int index = x*GRID_SIZE + y;
  
  if (index >= ACTUAL_SIZE) return;
  
  this->frontierGrid[index] = 127;
}


void FrontierMap::closeSquare(int x, int y){
  x /= SCALE;
  y /= SCALE;
  x = BOUNDARY + x;
  y = BOUNDARY - y;
  
  int index = x*GRID_SIZE + y;
  
  if (index >= ACTUAL_SIZE) return;
  
  if ((this->frontierGrid[index] > 0) && (this->frontierGrid[index] < 127)) this->frontierGrid[index] -= 1;
  else if (this->frontierGrid[index] <= 0) this->frontierGrid[index] += 1;
}


void FrontierMap::frontierSquare(int x, int y){
  x /= SCALE;
  y /= SCALE;
  x = BOUNDARY + x;
  y = BOUNDARY - y;
  
  int index = x*GRID_SIZE + y;
  
  if (index >= ACTUAL_SIZE) return;
  
  if ((this->frontierGrid[index] > -110) && (this->frontierGrid[index] < 127))
  this->frontierGrid[index] = -81;
}


int FrontierMap::getSquare(int x, int y){
  int index = x*GRID_SIZE + y;
  
  if (index >= ACTUAL_SIZE) return 0;
  else return this->frontierGrid[index];
}






void FrontierMap::sendToImage(char* filename){
  PngWriter* w = new PngWriter();
  int size = GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<GRID_SIZE; i++){
    for (int j=0; j<GRID_SIZE; j++) {
      int cur = this->getSquare(i, j);
      setImagePixel(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
}



void FrontierMap::setImagePixel(PngWriter* w, int x, int y, int value){
  int color;
  
  //open map square
  if (value > 80) {
    color = 0x0000ffff;
  
  //frontier map square
  } else if (value < -80) {
    color = 0xffff00ff;
  
  //unknown/closed map square
  } else {
    color = 0x808080ff;
  }
  
  w->set_pixel(x, y, color);
}
