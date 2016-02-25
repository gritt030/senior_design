
#include "houghgrid.h"

//constructor, create grid
HoughGrid::HoughGrid() {
  this->map = new unsigned short[GRID_SIZE * GRID_SIZE]();
  this->D_THETA = 3.141592654 / GRID_SIZE;
}

//destructor, destroy grid
HoughGrid::~HoughGrid() {
  delete[] this->map;
}


//set a value in the grid to a character
void HoughGrid::setValue(int x, int y, unsigned short value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=HoughGrid::GRID_SIZE) || (y>=HoughGrid::GRID_SIZE)) {
    return;
  }
  
  //set value
  this->map[y*HoughGrid::GRID_SIZE + x] = value;
}


//change a value in the grid by the amount passed in
void HoughGrid::changeValue(int x, int y, unsigned short value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=HoughGrid::GRID_SIZE) || (y>=HoughGrid::GRID_SIZE)) {
    return;
  }
  
  //int index = y*HoughGrid::GRID_SIZE + x;
  unsigned short current = this->map[y*HoughGrid::GRID_SIZE + x];
  unsigned short next = current + value;
  
  if (next < current) this->map[y*HoughGrid::GRID_SIZE + x] = MAX_VALUE;
  else this->map[y*HoughGrid::GRID_SIZE + x] = next;
}


//get the value of a square in the grid
//x=theta, y=radius
unsigned short HoughGrid::getValue(int x, int y){
  //bounds checking
  if ((x<0) || (y<0) || (x>=HoughGrid::GRID_SIZE) || (y>=HoughGrid::GRID_SIZE)) {
    return 0;
  }
  
  //return value
  return this->map[y*HoughGrid::GRID_SIZE + x];
}


void HoughGrid::addHoughPoint(int x, int y){
  double theta = 0.0;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    int r = (int)round((double)(x-CENTER)*cos(theta) + (double)(y-CENTER)*sin(theta));
    r += ADDITION;
    this->changeValue(i, r, (unsigned short)1);
    theta += D_THETA;
  }
}


void HoughGrid::removeHoughPoint(int x, int y){
  double theta = 0.0;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    int r = (int)round((double)(x-CENTER)*cos(theta) + (double)(y-CENTER)*sin(theta));
    r += ADDITION;
    
      //bounds checking
    if ((i<0) || (r<0) || (i>=HoughGrid::GRID_SIZE) || (r>=HoughGrid::GRID_SIZE)) {
      return;
    }
  
    //int index = y*HoughGrid::GRID_SIZE + x;
    unsigned short current = this->map[r*HoughGrid::GRID_SIZE + i];
    unsigned short next = current - 1;
    
    if (next > current) this->map[r*HoughGrid::GRID_SIZE + i] = 0;
    else this->map[r*HoughGrid::GRID_SIZE + i] = next;
    
    theta += D_THETA;
  }
}


void HoughGrid::addHoughPointWeighted(int x, int y, unsigned short weight){
  double theta = 0.0;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    int r = (int)round((double)(x-CENTER)*cos(theta) + (double)(y-CENTER)*sin(theta));
    r += ADDITION;
    this->changeValue(i, r, weight);
    theta += D_THETA;
  }
}

