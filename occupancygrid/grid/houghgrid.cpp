
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
    int r = (int)round((double)x*cos(theta) + (double)y*sin(theta));
    r += Grid::GRID_SIZE;
    this->changeValue(i, r, (unsigned short)1);
    theta += D_THETA;
  }
}


void HoughGrid::addHoughPointWeighted(int x, int y, unsigned short weight){
  double theta = 0.0;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    int r = (int)round((double)x*cos(theta) + (double)y*sin(theta));
    r += Grid::GRID_SIZE;
    this->changeValue(i, r, weight);
    theta += D_THETA;
  }
}


void HoughGrid::findMaxima(){
  this->maxima1();
  this->maxima2();
}


void HoughGrid::maxima1(){
  HoughGrid* newGrid = new HoughGrid();

  
  int ex,ey,wx,wy;
  unsigned short n,s,e,w,nw,ne,sw,se, cur;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    for (int j=1; j<HoughGrid::GRID_SIZE-1; j++) {
      ex = i+1;
      ey = j;
      wx = i-1;
      wy = j;
      
      if (i == 0) {
        wx = HoughGrid::GRID_SIZE-1;
        wy = 2*Grid::GRID_SIZE - j;
      } else if (i == HoughGrid::GRID_SIZE - 1) {
        ex = 0;
        ey = 2*Grid::GRID_SIZE - j;
      }
      
      int id = i*HoughGrid::GRID_SIZE;
      int wd = wx*HoughGrid::GRID_SIZE;
      int ed = ex*HoughGrid::GRID_SIZE;
      int jd = j;
      
      cur = this->map[id+jd];
      jd--;
      nw = this->map[wd+jd];
      ne = this->map[ed+jd];
      n = this->map[id+jd];
      jd+=2;
      sw = this->map[wd+jd];
      se = this->map[ed+jd];
      s = this->map[id+jd];
      w = this->map[wd+wy];
      e = this->map[ed+ey];
      
      /*
      n = this->getValue(i,j-1);
      s = this->getValue(i,j+1);
      e = this->getValue(ex,ey);
      w = this->getValue(wx,wy);
      nw = this->getValue(wx,j-1);
      ne = this->getValue(ex,j-1);
      sw = this->getValue(wx,j+1);
      se = this->getValue(ex,j+1);
      cur = this->getValue(i,j);
      */
      
      if ((cur>n) && (cur>s) && (cur>e) && (cur>w) && (cur>nw) && (cur>ne) && (cur>sw) && (cur>se)) {
        newGrid->setValue(i,j,cur);
      }
    }
  }
  
  delete[] this->map;
  this->map = newGrid->map;
  newGrid->map = (unsigned short*)nullptr;
  delete newGrid;
}


void HoughGrid::maxima2(){
  HoughGrid* newGrid = new HoughGrid();
  
  unsigned short* valArr = new unsigned short[NUM_PEAKS]();
  int* xArr = new int[NUM_PEAKS]();
  int* yArr = new int[NUM_PEAKS]();
  unsigned short min = 0;
  int mindex = 0;
  unsigned short cur;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      cur = this->map[i*HoughGrid::GRID_SIZE + j];
      if (cur > min) {
        valArr[mindex] = cur;
        xArr[mindex] = i;
        yArr[mindex] = j;
        mindex = submaxima1(valArr);
        min = valArr[mindex];
      }
    }
  }
  
  for (int i=0; i<NUM_PEAKS; i++){
    newGrid->setValue(xArr[i], yArr[i], valArr[i]);
  }
  
  delete[] valArr;
  delete[] xArr;
  delete[] yArr;
  
  delete[] this->map;
  this->map = newGrid->map;
  newGrid->map = (unsigned short*)nullptr;
  delete newGrid;
}


int HoughGrid::submaxima1(unsigned short* valArr){
      unsigned short min = valArr[0];
      int mindex = 0;
      
      for (int k=1; k<NUM_PEAKS; k++){
          if (valArr[k] < min) {
            min = valArr[k];
            mindex = k;
          }
      }
      
      return mindex;
}


