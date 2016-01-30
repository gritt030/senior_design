
#include "houghgrid.h"

//constructor, create grid
HoughGrid::HoughGrid() {
  this->map = new unsigned short[GRID_SIZE * GRID_SIZE]();
  this->D_THETA = 3.141592654 / GRID_SIZE;
}

//destructor, destroy grid
HoughGrid::~HoughGrid() {
  delete this->map;
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
  
  int index = y*HoughGrid::GRID_SIZE + x;
  unsigned short current = this->map[index];
  unsigned short next = current + value;
  
  if (next < current) this->map[index] = MAX_VALUE;
  else this->map[index] = next;
}


//get the value of a square in the grid
//x=theta, y=radius
unsigned short HoughGrid::getValue(int x, int y){
  //bounds checking
  if ((x<0) || (y<0) || (x>=HoughGrid::GRID_SIZE) || (y>=HoughGrid::GRID_SIZE)) {
    return 0;
  }
  
  //return value
  unsigned short value = this->map[y*HoughGrid::GRID_SIZE + x];
  return value;
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


//TODO: make this actually find the maxima
void HoughGrid::findMaxima(){
  HoughGrid* newGrid = new HoughGrid();
  
  unsigned short* valArr = new unsigned short[NUM_PEAKS]();
  int* xArr = new int[NUM_PEAKS]();
  int* yArr = new int[NUM_PEAKS]();
  int index = NUM_PEAKS - 1;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      unsigned short cur = this->getValue(i,j);
      
      if (cur >= valArr[index]) {
        bool check = false;
        
        for (int k=0; k<NUM_PEAKS; k++){
          if (check == false) {
            if ((abs(i-xArr[k]) < DISTANCE) || 
                (abs(j-yArr[k]) < DISTANCE) || 
                (abs(i-xArr[k]) > (GRID_SIZE - DISTANCE)) || 
                (abs(j-yArr[k]) > (GRID_SIZE - DISTANCE))) {
              check = true;
              if (valArr[k] < cur) {
                valArr[k] = cur;
                xArr[k] = i;
                yArr[k] = j;
              }
            }
          } else {
            if (valArr[k] > valArr[k-1]){
              unsigned short a = valArr[k-1];
              int b = xArr[k-1];
              int c = yArr[k-1];
              
              valArr[k-1] = valArr[k];
              xArr[k-1] = xArr[k];
              yArr[k-1] = yArr[k];
              valArr[k] = a;
              xArr[k] = b;
              yArr[k] = c;
            }
          }
        }
        
        if (check == false){
          valArr[index] = cur;
          xArr[index] = i;
          yArr[index] = j;
          index--;
          
          while ((index >= 0) && (cur > valArr[index])){
            valArr[index+1] = valArr[index];
            xArr[index+1] = xArr[index];
            yArr[index+1] = yArr[index];
            valArr[index] = cur;
            xArr[index] = i;
            yArr[index] = j;
            index--;
          }
          
          index = NUM_PEAKS - 1;
        }
      }
    }
  }
    
  for (int i=0; i<NUM_PEAKS; i++){
    /*
    std::cout << (int)valArr[i] << ", ";
    std::cout << xArr[i] << ", ";
    std::cout << yArr[i] << std::endl; //*/
    newGrid->setValue(xArr[i], yArr[i], valArr[i]);
  }
  
  delete[] valArr;
  delete[] xArr;
  delete[] yArr;
  
  /*
  int ex,ey,wx,wy;
  unsigned short n,s,e,w, cur;
  
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
      
      n = this->getValue(i,j-1);
      s = this->getValue(i,j+1);
      e = this->getValue(ex,ey);
      w = this->getValue(wx,wy);
      cur = this->getValue(i,j);
      
      if ((cur>n) && (cur>s) && (cur>e) && (cur>w)) {
        newGrid->setValue(i,j,cur);
      }
    }
  }
  //*/
  
  delete this->map;
  this->map = newGrid->map;
  newGrid->map = (unsigned short*)nullptr;
  delete newGrid;
}


