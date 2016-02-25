
#include "occupancygrid.h"

HoughGrid* OccupancyGrid::performHoughTransform(){
  HoughGrid* h = new HoughGrid();
  
  //generate hough transform grid
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE; j++){
      char cur = this->grid->getValue(i,j);
      if (cur < 0) h->addHoughPoint(i,j);//h->addHoughPointWeighted(i,j,(unsigned short)(-cur));
    }
  }
  
//   for (int i=0; i<Grid::GRID_SIZE; i++){
//     for (int j=0; j<Grid::GRID_SIZE; j++){
//       char cur = this->grid->getValue(i,j);
//       if (cur > 15) h->removeHoughPoint(i,j);
//     }
//   }
  
  return h;
}


OccupancyGrid* OccupancyGrid::generateHoughMap(){
  //do hough transform
  HoughGrid* h = this->performHoughTransform();
  HoughLeastSquares* s = new HoughLeastSquares(h);
  s->performLeastSquares();
  
  //generate new occupancy grid
  OccupancyGrid* g = new OccupancyGrid();
  
  //trace walls onto new occupancy grid
  this->traceHoughWalls(g, h, s);
  
  delete h;
  delete s;
  return g;
}


// void OccupancyGrid::traceHoughWalls(OccupancyGrid* newGrid, HoughGrid* hough){
//   hough->findMaxima();
//   
//   newGrid->X_Cardinal = hough->X_Cardinal;
//   newGrid->Y_Cardinal = hough->Y_Cardinal;
//   
//   this->traceCardinalDirections(hough->X_Cardinal, hough->Y_Cardinal, newGrid);
//     
//   double theta;
//   int radius;
//   
//   for (int i=0; i<HoughGrid::GRID_SIZE; i++){
//     for (int j=0; j<HoughGrid::GRID_SIZE; j++){
//       if (hough->getValue(i,j) != 0) {
//         theta = 3.141592654*((double)i/(double)(HoughGrid::GRID_SIZE));
//         radius = j - HoughGrid::ADDITION;
//         std::cout << theta << " " << radius << std::endl;
//         this->traceHoughLine((double)radius, theta, newGrid);
//       }
//     }
//   }
// }


void OccupancyGrid::traceHoughWalls(OccupancyGrid* newGrid, HoughGrid* hough, HoughLeastSquares* houghLS){
  
  newGrid->X_Cardinal = houghLS->X_Cardinal;
  newGrid->Y_Cardinal = houghLS->Y_Cardinal;
  
  this->traceCardinalDirections(houghLS->X_Cardinal, houghLS->Y_Cardinal, newGrid);
    
  double theta, lsTheta;
  int radius, lsRadius;
  
  int thetaSum=0, radiusSum=0;
  int t,r, index;
  
  for (int t=0; t<HoughGrid::GRID_SIZE; t++) {
    for (int r=0; r<HoughGrid::GRID_SIZE; r++) {
      index = t + r*HoughGrid::GRID_SIZE;
      if (houghLS->lsqrGrid->map[index] > 0) {
        theta = 3.141592654*((double)t/(double)(HoughGrid::GRID_SIZE));
        radius = r - HoughGrid::ADDITION;
        this->traceHoughLine((double)radius, theta, newGrid);
      }
    }
  }
  
  
//   for (int i=0; i<HoughGrid::GRID_SIZE; i+=HoughLeastSquares::LS_ANGLE) {
//     for (int j=0; j<HoughGrid::GRID_SIZE; j+=HoughLeastSquares::LS_RADIUS) {
//       thetaSum = -1;
//       radiusSum = -1;
//       
//       for (int k=0; k<HoughLeastSquares::LS_ANGLE; k++) {
//         for (int m=0; m<HoughLeastSquares::LS_RADIUS; m++) {
//           t = (i+k);
//           r = (j+m);
//           if ((t >= HoughGrid::GRID_SIZE) || (r >= HoughGrid::GRID_SIZE)) continue;
//           index = t + r*HoughGrid::GRID_SIZE;
//           if (houghLS->lsqrGrid->map[index] != 0) {
//             thetaSum = t;
//             radiusSum = r;
//           }
//         }
//       }
//       
//       if (thetaSum < 0) continue;
//                 
//       lsRadius = radiusSum - HoughGrid::ADDITION;
//       lsTheta = 3.141592654*((double)thetaSum/(double)(HoughGrid::GRID_SIZE));
//       
//       for (int k=0; k<HoughLeastSquares::LS_ANGLE; k++) {
//         for (int m=0; m<HoughLeastSquares::LS_RADIUS; m++) {
//           t = (i+k);
//           r = (j+m);
//           if ((t >= HoughGrid::GRID_SIZE) || (r >= HoughGrid::GRID_SIZE)) continue;
//           index = t + r*HoughGrid::GRID_SIZE;
//           if (hough->map[index] != 0) {
//             theta = 3.141592654*((double)t/(double)(HoughGrid::GRID_SIZE));
//             radius = r - HoughGrid::ADDITION;
//             //this->traceHoughLine((double)radius, theta, (double)lsRadius, lsTheta, newGrid);
//             this->traceHoughLine((double)lsRadius, lsTheta, newGrid);
//             //this->traceHoughLine((double)radius, theta, (double)radius, theta, newGrid);
//             //this->traceHoughLine((double)radius, theta, newGrid);
//           }
//         }
//       }
//     }
//   }
}




void OccupancyGrid::traceHoughLine(double radius, double theta, double lsRadius, double lsTheta, OccupancyGrid* newGrid){  
  double lsy, lsx;
  double x, y, i;
  double lss = sin(lsTheta);
  double lsc = cos(lsTheta);
  double s = sin(theta);
  double c = cos(theta);
  int count = BRIDGE;
  int count2 = 0;
      
  //line more horizontal than vertical
  if ((theta > 0.7853981634) && (theta < 2.35619449)) {
    for (x=0; x<Grid::GRID_SIZE; x++){
      y = (radius-(x-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
      lsy = (lsRadius-(x-HoughGrid::CENTER)*lsc)/lss + HoughGrid::CENTER;
      //newGrid->grid->setValue((int)x, (int)lsy, -10); //TODO: Remove
      
      if (this->grid->getValue((int)x, (int)y) < 0) {
        count2++;
        
        if (count >= BRIDGE) {
          count2 = 0;
          count = 0;
          
        } else if (count2 < MIN_BRIDGE) {
          continue;
          
        } else if (count2 = MIN_BRIDGE) {
          for (i = (x-count2-count); i<=x; i++){
            y = (radius-(i-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
            lsy = (lsRadius-(i-HoughGrid::CENTER)*lsc)/lss + HoughGrid::CENTER;
            newGrid->grid->setValue((int)i, (int)lsy, -10);
          }
          count = 0;
          
        } else if (count > 0) {
          for (i = (x-count); i<=x; i++){
            y = (radius-(i-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
            lsy = (lsRadius-(i-HoughGrid::CENTER)*lsc)/lss + HoughGrid::CENTER;
            newGrid->grid->setValue((int)i, (int)lsy, -10);
          }
          count = 0;
          
        } else {
          newGrid->grid->setValue((int)x, (int)lsy, -10);
        }
        
      } else {
        count++;
      }
    }
  
  //line more vertical than horizontal
  } else {
    for (y=0; y<Grid::GRID_SIZE; y++){
      x = (radius-(y-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
      lsx = (lsRadius-(y-HoughGrid::CENTER)*lss)/lsc + HoughGrid::CENTER;
      //newGrid->grid->setValue((int)lsx, (int)y, -10); //TODO:remove
            
      if (this->grid->getValue((int)x, (int)y) < 0) {
        count2++;
        
        if (count >= BRIDGE) {
          count2 = 0;
          count = 0;
          
        } else if (count2 < MIN_BRIDGE) {
          continue;
          
        } else if (count2 = MIN_BRIDGE) {
          for (i = (y-count2-count); i<=y; i++){
            x = (radius-(i-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
            lsx = (lsRadius-(i-HoughGrid::CENTER)*lss)/lsc + HoughGrid::CENTER;
            newGrid->grid->setValue((int)lsx, (int)i, -10);
          }
          count = 0;
          
        } else if (count > 0) {
          for (i = (y-count); i<=y; i++){
            x = (radius-(i-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
            lsx = (lsRadius-(i-HoughGrid::CENTER)*lss)/lsc + HoughGrid::CENTER;
            newGrid->grid->setValue((int)lsx, (int)i, -10);
          }
          count = 0;
          
        } else {
          newGrid->grid->setValue((int)lsx, (int)y, -10);
        }
        
      } else {
        count++;
      }
    }
  }
}






void OccupancyGrid::traceHoughLine(double radius, double theta, OccupancyGrid* newGrid){
  double x, y, i;
  double s = sin(theta);
  double c = cos(theta);
  int count = BRIDGE;
  int count2 = 0;
  
  //line more horizontal than vertical
  if ((theta > 0.7853981634) && (theta < 2.35619449)) {
    for (x=0; x<Grid::GRID_SIZE; x++){
      y = (radius-(x-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
      
      if (this->grid->getValue((int)x, (int)y) < 0) {
        count2++;
        
        if (count >= BRIDGE) {
          count2 = 0;
          count = 0;
          
        } else if (count2 < MIN_BRIDGE) {
          continue;
          
        } else if (count2 = MIN_BRIDGE) {
          for (i = (x-count2-count); i<=x; i++){
            y = (radius-(i-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
            newGrid->grid->setValue((int)i, (int)y, -10);
          }
          count = 0;
          
        } else if (count > 0) {
          for (i = (x-count); i<=x; i++){
            y = (radius-(i-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
            newGrid->grid->setValue((int)i, (int)y, -10);
          }
          count = 0;
          
        } else {
          newGrid->grid->setValue((int)x, (int)y, -10);
        }
        
      } else {
        count++;
      }
    }
  
  //line more vertical than horizontal
  } else {
    for (y=0; y<Grid::GRID_SIZE; y++){
      x = (radius-(y-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
      
      if (this->grid->getValue((int)x, (int)y) < 0) {
        count2++;
        
        if (count >= BRIDGE) {
          count2 = 0;
          count = 0;
          
        } else if (count2 < MIN_BRIDGE) {
          continue;
          
        } else if (count2 = MIN_BRIDGE) {
          for (i = (y-count2-count); i<=y; i++){
            x = (radius-(i-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
            newGrid->grid->setValue((int)x, (int)i, -10);
          }
          count = 0;
          
        } else if (count > 0) {
          for (i = (y-count); i<=y; i++){
            x = (radius-(i-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
            newGrid->grid->setValue((int)x, (int)i, -10);
          }
          count = 0;
          
        } else {
          newGrid->grid->setValue((int)x, (int)y, -10);
        }
        
      } else {
        count++;
      }
    }
  }
}










void OccupancyGrid::traceCardinalDirections(double x_card, double y_card, OccupancyGrid* newGrid){
  double theta = x_card;
  
  double x, y;
  double s = sin(theta);
  double c = cos(theta);
  
  for (x=0; x<Grid::GRID_SIZE; x++){
    y = ((HoughGrid::CENTER-x)*c)/s + HoughGrid::CENTER;
    newGrid->grid->setValue((int)x, (int)y, -Grid::MAX_VALUE);
  }
  
  theta = y_card;
  s = sin(theta);
  c = cos(theta);
  
  for (y=0; y<Grid::GRID_SIZE; y++){
    x = ((HoughGrid::CENTER-y)*s)/c + HoughGrid::CENTER;
    newGrid->grid->setValue((int)x, (int)y, -Grid::MAX_VALUE); 
  }
}


void OccupancyGrid::sendHoughToImage(char* filename){
  HoughGrid* h = this->performHoughTransform();
  
  PngWriter* w = new PngWriter();
  int size = HoughGrid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      unsigned short cur = h->getValue(i, j);
      setImagePixelHough(w, i, j, cur*500);
    }
  }
  
  w->output_image();
  delete w;
  delete h;
}


void OccupancyGrid::sendHoughMaximaToImage(char* filename){
  HoughGrid* h = this->performHoughTransform();
  HoughLeastSquares* s = new HoughLeastSquares(h);
  s->findMaxima();
  
  PngWriter* w = new PngWriter();
  int size = HoughGrid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      unsigned short cur = s->lsqrGrid->getValue(i, j);
      setImagePixelHough(w, i, j, cur);
    }
  }
  
//   s->performLeastSquares();
//     
//   for (int i=0; i<HoughGrid::GRID_SIZE; i++){
//     for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
//       unsigned short cur = s->lsqrGrid->getValue(i, j);
//       if (cur > 0) setImagePixelHough(w, i, j, 65535);
//     }
//   }
  
  s->performLeastSquares();
    
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      unsigned short cur = s->lsqrGrid->getValue(i, j);
      if (cur > 0) setImagePixelHough(w, i, j, 65535);
    }
  }
  
  w->output_image();
  delete w;
  delete h;
  delete s;
}


void OccupancyGrid::setImagePixelHough(PngWriter* w, int x, int y, unsigned short value){
  int color;
  
  unsigned char r, g, b;
  
  if (value >= 49152) {r=0xff; g=255-(value-49152)/64; b=0x00;}
  
  else if (value >= 32768) {r=(value-32768)/64; g=0xff; b=0x00;}
  
  else if (value >= 16384) {r=0x00; g=0xff; b=255-(value-16384)/64;}
  
  else if (value > 0) {r=0x00; g=value/64; b=0xff;}
  
  else {r=0x00; g=0x00; b=0x00;}
  
  color = 0x000000ff;
  color |= r<<24;
  color |= g<<16;
  color |= b<<8;
  
  w->set_pixel(x, y, color);
}
