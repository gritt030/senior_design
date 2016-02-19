
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


void HoughGrid::addHoughPointWeighted(int x, int y, unsigned short weight){
  double theta = 0.0;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    int r = (int)round((double)(x-CENTER)*cos(theta) + (double)(y-CENTER)*sin(theta));
    r += ADDITION;
    this->changeValue(i, r, weight);
    theta += D_THETA;
  }
}


void HoughGrid::findMaxima(){
  this->maxima1();
  this->maxima2();
  
  this->detectHist();
  this->leastSquares();
}

//Get all local maxima
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
        wy = 2*ADDITION - j;
      } else if (i == HoughGrid::GRID_SIZE - 1) {
        ex = 0;
        ey = 2*ADDITION - j;
      }
      
      n = this->getValue(i,j-1);
      s = this->getValue(i,j+1);
      e = this->getValue(ex,ey);
      w = this->getValue(wx,wy);
      nw = this->getValue(wx,j-1);
      ne = this->getValue(ex,j-1);
      sw = this->getValue(wx,j+1);
      se = this->getValue(ex,j+1);
      cur = this->getValue(i,j);
      
      
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


//get NUM_PEAKS highest maxima
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
      cur = this->map[i + j*HoughGrid::GRID_SIZE];
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


//get cardinal direction of building and save in global vars
void HoughGrid::detectHist() {
  int* hist = new int[360]();
  int index = 1;
  int theta;
  double curAngle = 0.0;
  int curSum = 0;
  int curNum = 0;
  
  //generate histogram inside hist
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++){
      unsigned short val = this->map[j*GRID_SIZE + i];
      if (val != 0) {
        curSum += val;
        curNum++;
      }
    }
    
    curAngle += D_THETA;
      
      if ((curAngle * 57.29577951) >= ((float)index / 2.0)) {
        if (curNum != 0) hist[index - 1] = curSum/curNum;
        index++;
        curSum = 0;
        curNum = 0;
      }
  }
  
  int maxInd = 0;
  
  //find maximum in maxInd
  for (int i=1; i<360; i++){
    if (hist[i] > hist[maxInd]) maxInd = i;
  }
  
  int index2;
  int histWeight1 = 0;
  int histWeight2 = 0;
  int thetaWeight1 = 0;
  int thetaWeight2 = 0;
  
  for (int i=(maxInd-10); i<=(maxInd+10); i++) {
    if (i < 0) {
      index = i+360;
      index2 = index - 180;
    } else {
      index = i % 360;
      index2 = (i + 180) % 360;
    }
    
    histWeight1 += hist[index];
    histWeight2 += hist[index2];
    thetaWeight1 += i*hist[index];
    thetaWeight2 += i*hist[index2];
    
  }
  
  //make sure that there were actually peaks in the hough transform
  if ((histWeight1 + histWeight2) == 0) {
    X_Cardinal = 1.570796327;
    Y_Cardinal = 0.0;
    delete[] hist;
    return;
  }
  
  //least squares for finding best cardinal direction fit
  theta = (thetaWeight1 + thetaWeight2) / (histWeight1 + histWeight2);
  
  //save cardinal directions
  if ((theta > 90) && (theta < 270)) {
    X_Cardinal = (double)theta * 0.0087266463;
    Y_Cardinal = (double)((theta + 180) % 360) * 0.0087266463;
  } else {
    X_Cardinal = (double)((theta + 180) % 360) * 0.0087266463;
    Y_Cardinal = (double)theta * 0.0087266463;
  }
  
  //std::cout << maxInd/2 << " " << maxInd2/2 << std::endl;
  
  delete[] hist;
}


void HoughGrid::leastSquares(){
  int cur;
  int thetaSum=0, radiusSum=0, weightSum=0, num=0;
  int t,r, index;

  for (int i=0; i<GRID_SIZE; i+=LS_ANGLE) {
    for (int j=0; j<GRID_SIZE; j+=LS_RADIUS) {

      for (int k=0; k<LS_ANGLE; k++) {
        for (int m=0; m<LS_RADIUS; m++) {
          t = (i+k);
          r = (j+m);
          index = (t % GRID_SIZE) + (r % GRID_SIZE)*GRID_SIZE;
          
          cur = (int)this->map[index];
        
          if (cur > 0) {
            thetaSum += t*cur;
            radiusSum += r*cur;
            weightSum += cur;
            num++;
            this->map[index] = 0;
          }
        }
      }
      if (num > 0) {
        /*
        std::cout << thetaSum << " ";
        std::cout << radiusSum << " ";
        std::cout << weightSum << " ";
        std::cout << num << " ";
        std::cout << " " << sumT << " ";
        std::cout << sumR << std::endl;
        */
        thetaSum /= (weightSum);
        radiusSum /= (weightSum);
        this->map[(thetaSum % GRID_SIZE) + (radiusSum % GRID_SIZE)*GRID_SIZE] = weightSum;
      }
      thetaSum = 0;
      radiusSum = 0;
      weightSum = 0;
      num = 0;
    }
  }
}
