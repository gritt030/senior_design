#include "houghleastsquares.h"

HoughLeastSquares::HoughLeastSquares(HoughGrid* grid){
  this->houghGrid = grid;
}


HoughLeastSquares::~HoughLeastSquares(){
  if (this->lsqrGrid != nullptr) delete this->lsqrGrid;
}


void HoughLeastSquares::performLeastSquares(){
  findMaxima();
  
//   leastSquare(3,3);
//   leastSquare(5,5);
//   leastSquare(7,7);
//   leastSquare(11,11);
//   leastSquare(17,17);
//   leastSquare(23,23);
//   leastSquare(29,29);
//   leastSquare(37,37);
  //leastSquare(10,10);
  
  lsqrGrid->X_Cardinal = X_Cardinal;
  lsqrGrid->Y_Cardinal = Y_Cardinal;
  
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
  mergePeaks();
}


void HoughLeastSquares::leastSquare(int radSize, int angSize){
  HoughGrid* hough = this->lsqrGrid;
  HoughGrid* newLS = new HoughGrid();
  
  double theta, lsTheta;
  int radius, lsRadius;
  
  int cur;
  int thetaSum, radiusSum, weightSum, num;
  int t,r, index;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i+=angSize) {
    for (int j=0; j<HoughGrid::GRID_SIZE; j+=radSize) {
      thetaSum=0;
      radiusSum=0;
      weightSum=0;
      num=0;
      
      for (int k=0; k<angSize; k++) {
        for (int m=0; m<radSize; m++) {
          t = (i+k);
          r = (j+m);
          
          if (t >= HoughGrid::GRID_SIZE) {
            if ((2*HoughGrid::ADDITION - r) >= HoughGrid::GRID_SIZE) continue;
            else if ((2*HoughGrid::ADDITION - r) < 0) continue;
            index = (t%HoughGrid::GRID_SIZE) + (2*HoughGrid::ADDITION - r)*HoughGrid::GRID_SIZE;
          } else {
            if (r >= HoughGrid::GRID_SIZE) continue;
            index = t + r*HoughGrid::GRID_SIZE;
          }
          
          cur = (int)hough->map[index];
        
          if (cur > 0) {
            thetaSum += t*cur;
            radiusSum += r*cur;
            weightSum += cur;
            num++;
          }
        }
      }
      
      if (num > 0) {
        thetaSum /= (weightSum);
        radiusSum /= (weightSum);
        weightSum /= num;
        
        if (thetaSum >= HoughGrid::GRID_SIZE) {
          radiusSum = 2*HoughGrid::ADDITION - radiusSum;
        }
        
        if (radiusSum >= HoughGrid::GRID_SIZE) continue;
        else if (radiusSum < 0) continue;
        
        newLS->setValue((thetaSum % HoughGrid::GRID_SIZE), radiusSum, (unsigned short)weightSum);
      }
    }
  }
  
  if (this->lsqrGrid != nullptr) delete this->lsqrGrid;
  this->lsqrGrid = newLS;
}






void HoughLeastSquares::mergePeaks(){
  HoughGrid* hough = this->houghGrid;
  HoughGrid* curLS = this->lsqrGrid;
  HoughGrid* newLS = new HoughGrid();
  
  int theta, radius;
  
  int cur, cur2;
  int index, index2;
  int* buf = new int[2];
  int delta, height;
  int count = 0;
  long vall = 0;
  int cnt2 = 0;
    
  for (int t=0; t<HoughGrid::GRID_SIZE; t++) {
    for (int r=0; r<HoughGrid::GRID_SIZE; r++) {
      index = t + r*HoughGrid::GRID_SIZE;
      cur = (int)curLS->map[index];
      
      if (cur > 0) {        
        buf[0] = t;
        buf[1] = r;
        getClosestPeak(buf);
        
        index2 = buf[0] + buf[1]*HoughGrid::GRID_SIZE;
        cur2 = (int)curLS->map[index];
                
        height = abs(cur - cur2) + 1;        
        delta = getDelta(t,r, buf[0], buf[1]);
        
        if (((t-buf[0])*(t-buf[0]) + (r-buf[1])*(r-buf[1])) > 22500) {
          vall += (delta/height);
          cnt2++;
          newLS->setValue((t % HoughGrid::GRID_SIZE), r, (unsigned short)cur);
          
        } else if ((delta / height) < 76) {
          theta = t*cur + buf[0]*cur2;
          radius = r*cur + buf[1]*cur2;
          theta /= (cur + cur2);
          radius /= (cur + cur2);
          count++;
          newLS->setValue((theta % HoughGrid::GRID_SIZE), radius, (unsigned short)((cur + cur2) / 2));
          
        } else {
          vall += (delta/height);
          cnt2++;
          newLS->setValue((t % HoughGrid::GRID_SIZE), r, (unsigned short)cur);
        }
      }
    }
  }
  
  delete[] buf;
  if (this->lsqrGrid != nullptr) delete this->lsqrGrid;
  this->lsqrGrid = newLS;
  
  std::cout << "Total merged: " << count << " " << vall/(cnt2+1) << std::endl;
}


void HoughLeastSquares::getClosestPeak(int* buf){
  int curT = buf[0];
  int curR = buf[1];
  
  int newT = curT-1;
  int newR = curR-1;
  
  int perimeter = 7;
  int side = 2;
  int curPer = 0;
  int curSide = 0;
  
  int cur=0, index;
  int theta, radius;
  char count = 0;
  
  while (true) {
    if (newT < 0) {
      theta = newT + HoughGrid::GRID_SIZE;
      radius = 2*HoughGrid::ADDITION - newR;
    } else if (newT >= HoughGrid::GRID_SIZE) {
      theta = newT - HoughGrid::GRID_SIZE;
      radius = 2*HoughGrid::ADDITION - newR;
    } else {
      theta = newT;
      radius = newR;
    }
    
    //std::cout << "        " << newT << " " << theta << " " << newR << " " << radius << " " << (int)count << std::endl;
    
    if ((radius < 0) || (radius >= HoughGrid::GRID_SIZE)) continue;
    index = theta + radius*HoughGrid::GRID_SIZE;
    
    cur = this->lsqrGrid->map[index];
    
    if (cur > 0) {
      buf[0] = theta;
      buf[1] = radius;
      return;
    }
    
    if (curPer == perimeter){
      perimeter += 8;
      side += 2;
      newT -= 1;
      newR -= 2;
      count = 0;
      curSide = 0;
      curPer = 0;
      
    } else if (curSide == side){
      count++;
      curSide = 1;
      curPer++;
      if (count == 1) newR++;
      else if (count == 2) newT--;
      else if (count == 3) newR--;
      
    } else {
      curSide++;
      curPer++;
      if (count == 0) newT++;
      else if (count == 1) newR++;
      else if (count == 2) newT--;
      else if (count == 3) newR--;
    }
  }
}


int HoughLeastSquares::getDelta(int t1, int r1, int t2, int r2){
  int val = HoughGrid::GRID_SIZE / 4;
  int ct, cr;
  
  if ((t1 < val) && (t2 > val*3)){
    ct = -1;
    cr = (2*HoughGrid::ADDITION - r2) - r1;
    if (cr != 0) cr /= abs(cr);
  } else if ((t2 < val) && (t1 > val*3)){
    ct = 1;
    cr = (2*HoughGrid::ADDITION - r2) - r1;
    if (cr != 0) cr /= abs(cr);
  } else {
    if (t2 == t1) ct = 0;
    else ct = (t2 - t1)/abs(t2 - t1);
    
    if (r2 == r1) cr = 0;
    else cr = (r2 - r1)/abs(r2 - r1);
  }
  
  
  int index = t1 + r1*HoughGrid::GRID_SIZE;
  int max, min, cur;
  int t = t1;
  int r = r1;
  int dt, dr;
  
  max = (int)this->lsqrGrid->map[index];
  min = max;
  
  while ((t != t2) || (r != r2)){
    if ((t < val) && (t2 > val*3)){
      dt = (t2 - HoughGrid::GRID_SIZE) - t;
      dr = (2*HoughGrid::ADDITION - r2) - r;
      
    } else if ((t2 < val) && (t > val*3)){
      dt = (t2 + HoughGrid::GRID_SIZE) - t;
      dr = (2*HoughGrid::ADDITION - r2) - r;
            
    } else {
      dt = t2 - t;
      dr = r2 - r;
      if (t2 == t) ct = 0;
      else ct = (t2 - t)/abs(t2 - t);
    
      if (r2 == r) cr = 0;
      else cr = (r2 - r)/abs(r2 - r);
    }
    
    if (abs(dt) > abs(dr)) t += ct;
    else r += cr;
    
    if (t < 0){
      t += HoughGrid::GRID_SIZE;
      r = 2*HoughGrid::ADDITION - r;
      cr *= -1;
      
    } else if (t >= HoughGrid::GRID_SIZE){
      t -= HoughGrid::GRID_SIZE;
      r = 2*HoughGrid::ADDITION - r;
      cr *= -1;
      
    } else if ((r < 0) || (r >= HoughGrid::GRID_SIZE)) continue;
    
    index = t + r*HoughGrid::GRID_SIZE;
    cur = (int)this->lsqrGrid->map[index];
    
    if (cur < min) min = cur;
    else if (cur > max) max = cur;
  }
  
  index = t2 + r2*HoughGrid::GRID_SIZE;
  
  if ((int)this->lsqrGrid->map[index] > max) max = (int)this->lsqrGrid->map[index];
  else if ((int)this->lsqrGrid->map[index] < min) min = (int)this->lsqrGrid->map[index];
  
  return max - min;
}









void HoughLeastSquares::findMaxima(){
  this->maxima1();
  this->maxima2();
  
  this->detectHist();
  this->houghGrid->X_Cardinal = X_Cardinal;
  this->houghGrid->Y_Cardinal = Y_Cardinal;
  this->lsqrGrid->X_Cardinal = X_Cardinal;
  this->lsqrGrid->Y_Cardinal = Y_Cardinal;
}

//Get all local maxima
void HoughLeastSquares::maxima1(){
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
        wy = 2*HoughGrid::ADDITION - j;
      } else if (i == HoughGrid::GRID_SIZE - 1) {
        ex = 0;
        ey = 2*HoughGrid::ADDITION - j;
      }
      
      n = this->houghGrid->getValue(i,j-1);
      s = this->houghGrid->getValue(i,j+1);
      e = this->houghGrid->getValue(ex,ey);
      w = this->houghGrid->getValue(wx,wy);
      nw = this->houghGrid->getValue(wx,j-1);
      ne = this->houghGrid->getValue(ex,j-1);
      sw = this->houghGrid->getValue(wx,j+1);
      se = this->houghGrid->getValue(ex,j+1);
      cur = this->houghGrid->getValue(i,j);
      
      
//       if ((cur>n) && (cur>s) && (cur>e) && (cur>w) && (cur>nw) && (cur>ne) && (cur>sw) && (cur>se)) {
//         newGrid->setValue(i,j,cur);
//       }
      
      if ((cur>=n) && (cur>=s) && (cur>=e) && (cur>=w) && (cur>=nw) && (cur>=ne) && (cur>=sw) && (cur>=se)) {
        newGrid->setValue(i,j,cur);
      }
    }
  }
  
  if (this->lsqrGrid != nullptr) delete this->lsqrGrid;
  this->lsqrGrid = newGrid;
}


//get NUM_PEAKS highest maxima
void HoughLeastSquares::maxima2(){
  HoughGrid* newGrid = new HoughGrid();
  
  unsigned short* valArr = new unsigned short[NUM_PEAKS]();
  int* xArr = new int[NUM_PEAKS]();
  int* yArr = new int[NUM_PEAKS]();
  unsigned short min = 0;
  int mindex = 0;
  unsigned short cur;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++) {
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      cur = this->lsqrGrid->map[i + j*HoughGrid::GRID_SIZE];
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
  
  delete this->lsqrGrid;
  this->lsqrGrid = newGrid;
}


int HoughLeastSquares::submaxima1(unsigned short* valArr){
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
void HoughLeastSquares::detectHist() {
  int* hist = new int[360]();
  int index = 1;
  int theta;
  double curAngle = 0.0;
  int curSum = 0;
  int curNum = 0;
  
  //generate histogram inside hist
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++){
      unsigned short val = this->lsqrGrid->map[j*HoughGrid::GRID_SIZE + i];
      if (val != 0) {
        curSum += val;
        curNum++;
      }
    }
    
    curAngle += this->lsqrGrid->D_THETA;
      
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









// void HoughGrid::leastSquares(){
//   int cur;
//   int thetaSum=0, radiusSum=0, weightSum=0, num=0;
//   int t,r, index;
// 
//   for (int i=0; i<GRID_SIZE; i+=LS_ANGLE) {
//     for (int j=0; j<GRID_SIZE; j+=LS_RADIUS) {
// 
//       for (int k=0; k<LS_ANGLE; k++) {
//         for (int m=0; m<LS_RADIUS; m++) {
//           t = (i+k);
//           r = (j+m);
//           index = (t % GRID_SIZE) + (r % GRID_SIZE)*GRID_SIZE;
//           
//           cur = (int)this->map[index];
//         
//           if (cur > 0) {
//             thetaSum += t*cur;
//             radiusSum += r*cur;
//             weightSum += cur;
//             num++;
//             this->map[index] = 0;
//           }
//         }
//       }
//       if (num > 0) {
//         thetaSum /= (weightSum);
//         radiusSum /= (weightSum);
//         this->map[(thetaSum % GRID_SIZE) + (radiusSum % GRID_SIZE)*GRID_SIZE] = weightSum;
//       }
//       thetaSum = 0;
//       radiusSum = 0;
//       weightSum = 0;
//       num = 0;
//     }
//   }
// }