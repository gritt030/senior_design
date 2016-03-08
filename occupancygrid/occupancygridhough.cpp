
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


// OccupancyGrid* OccupancyGrid::generateHoughMap(){
//   //do hough transform
//   HoughGrid* h = this->performHoughTransform();
//   HoughLeastSquares* s = new HoughLeastSquares(h);
//   s->performLeastSquares();
//   
//   //generate new occupancy grid
//   OccupancyGrid* g = new OccupancyGrid();
//   
//   //trace walls onto new occupancy grid
//   this->traceHoughWalls(g, h, s);
//   
//   delete h;
//   delete s;
//   return g;
// }







//new hough stuff
OccupancyGrid* OccupancyGrid::generateHoughMap(){
  //do hough transform
  HoughGrid* h = this->performHoughTransform();
  HoughLeastSquares* s = new HoughLeastSquares(h);
  s->findMaxima();
  
  //generate new occupancy grid
  OccupancyGrid* g = new OccupancyGrid();
  
  //trace walls onto new occupancy grid
  this->traceHoughWalls(g, s);
  
  delete h;
  delete s;
  return g;
}


void OccupancyGrid::traceHoughWalls(OccupancyGrid* newGrid, HoughLeastSquares* houghLS){
  newGrid->X_Cardinal = houghLS->X_Cardinal;
  newGrid->Y_Cardinal = houghLS->Y_Cardinal;
  
  this->traceCardinalDirections(houghLS->X_Cardinal, houghLS->Y_Cardinal, newGrid);
    
  double theta, lsTheta;
  int radius, lsRadius;
  
  int thetaSum=0, radiusSum=0;
  int t,r, index;
  HoughLine* curLines = nullptr;
  
  int totLines = 0;
  int merLines = 0;
  
  std::cout << "Finding lines:\n";
  
  for (int t=0; t<HoughGrid::GRID_SIZE; t++) {
    for (int r=0; r<HoughGrid::GRID_SIZE; r++) {
      index = t + r*HoughGrid::GRID_SIZE;
      if (houghLS->lsqrGrid->map[index] > 0) {
        theta = 3.141592654*((double)t/(double)(HoughGrid::GRID_SIZE));
        radius = r - HoughGrid::ADDITION;
        curLines = this->getHoughLines(radius, theta);
        if (curLines != nullptr) {
          totLines += this->combineHoughLines(curLines);
          merLines += this->mergeLineList(curLines);
        }
      }
    }
  }
  
  std::cout << "Total lines: " << totLines << std::endl;
  std::cout << "Merged Lines: " << merLines << std::endl;
  
  int line = 0;
  HoughLine* curLine = lineList;
  while (curLine != nullptr){
    this->drawHoughLine(curLine, newGrid);
    curLine = curLine->next;
    line++;
  }
  
  std::cout << "Drawn Lines: " << line << std::endl;
}


void OccupancyGrid::drawHoughLine(HoughLine* line, OccupancyGrid* newGrid){
  double x, y;
  double theta = line->theta;
  double radius = line->radius;
  
  double s = sin(theta);
  double c = cos(theta);
  
  //line more horizontal than vertical
  if ((theta > 0.7853981634) && (theta < 2.35619449)) {
    for (x=(line->startX); x<(line->endX); x++){
      y = (radius-(x-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
      newGrid->grid->setValue((int)x, (int)y, -10);
    }
  
  //line more vertical than horizontal
  } else {
    for (y=(line->startY); y<(line->endY); y++){
      x = (radius-(y-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
      newGrid->grid->setValue((int)x, (int)y, -10);
    }
  }
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









//generate list of lines for a given radius and theta value
HoughLine* OccupancyGrid::getHoughLines(double radius, double theta){
  double x, y;
  double s = sin(theta);
  double c = cos(theta);

  int numLines = 0;
  bool newLine = true;
  int startX, startY, endX, endY;
  HoughLine* lines = nullptr;
  HoughLine* retLine = nullptr;
  HoughLine* curLine;
  
  //line more horizontal than vertical
  if ((theta > 0.7853981634) && (theta < 2.35619449)) {
    for (x=0; x<Grid::GRID_SIZE; x++){
      y = (radius-(x-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
      
      if (this->grid->getValue((int)x, (int)y) < 0) {
        if (newLine) {
          startX = x;
          startY = y;
          endX = x;
          endY = y;
          newLine = false;
        } else {
          endX = x;
          endY = y;
        }
        
      } else {
        if (!newLine){
          curLine = new HoughLine();
          makeHoughLine(radius, theta, startX, startY, endX, endY, curLine);
          numLines++;
          
          if (lines == nullptr){
            lines = curLine;
            retLine = curLine;
          } else {
            lines->next = curLine;
            lines = curLine;
          }
          
          newLine = true;
        }
      }
    }
  
  //line more vertical than horizontal
  } else {
    for (y=0; y<Grid::GRID_SIZE; y++){
      x = (radius-(y-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
      
      if (this->grid->getValue((int)x, (int)y) < 0) {
        if (newLine) {
          startX = x;
          startY = y;
          endX = x;
          endY = y;
          newLine = false;
        } else {
          endX = x;
          endY = y;
        }
        
      } else {
        if (!newLine){
          curLine = new HoughLine();
          makeHoughLine(radius, theta, startX, startY, endX, endY, curLine);
          numLines++;
          
          if (lines == nullptr){
            lines = curLine;
            retLine = curLine;
          } else {
            lines->next = curLine;
            lines = curLine;
          }
          
          newLine = true;
        }
      }
    }
  }
  
  return retLine;
}


//combine lines generated by getHoughLines
int OccupancyGrid::combineHoughLines(HoughLine* lines){
  HoughLine* prevPrevLine = nullptr;
  HoughLine* prevLine = lines;
  HoughLine* curLine = lines->next;
  int xDiff, yDiff, diff;
  int numLines = 0;
    
  while (curLine != nullptr){
    xDiff = (curLine->startX) - (prevLine->endX);
    yDiff = (curLine->startY) - (prevLine->endY);
    diff = xDiff*xDiff + yDiff*yDiff;
        
    //if this line and preline are close, make into one line
    if (diff < BRIDGE){
      prevLine->endX = curLine->endX;
      prevLine->endY = curLine->endY;
      prevLine->next = curLine->next;
      delete curLine;
      curLine = prevLine->next;
    
    //make sure prevline is long enough to keep
    } else {
      xDiff = (prevLine->endX) - (prevLine->startX);
      yDiff = (prevLine->endY) - (prevLine->startY);
      diff = xDiff*xDiff + yDiff*yDiff;
      
      if (diff < MIN_BRIDGE){
        prevLine->startX = curLine->startX;
        prevLine->startY = curLine->startY;
        prevLine->endX = curLine->endX;
        prevLine->endY = curLine->endY;
        prevLine->next = curLine->next;
        delete curLine;
        curLine = prevLine->next;
        
      } else {
        numLines++;
        prevPrevLine = prevLine;
        prevLine = curLine;
        curLine = curLine->next;
      }
    }
  }
  
  //check that last line created is long enough to keep
  xDiff = (prevLine->endX) - (prevLine->startX);
  yDiff = (prevLine->endY) - (prevLine->startY);
  diff = xDiff*xDiff + yDiff*yDiff;
  
  if (diff < MIN_BRIDGE){
    if (prevPrevLine == nullptr){
      delete lines;
      return 0;
      
    } else {
      delete prevLine;
      prevPrevLine->next = nullptr;
      numLines--;
    }    
  }
  
  return numLines;
}


//create a new hough line with the given parameters
void OccupancyGrid::makeHoughLine(double r, double t, int sX, int sY, int eX, int eY, HoughLine* hl){
  ///hl = new HoughLine;
  hl->radius = r;
  hl->theta = t;
  hl->startX = sX;
  hl->startY = sY;
  hl->endX = eX;
  hl->endY = eY;
  hl->next = nullptr;
}


int OccupancyGrid::mergeLineList(HoughLine* lines){
  if (lineList == nullptr){
    lineList = lines;
    int cnt = 0;
    HoughLine* cur = lines;
    while (cur != nullptr){
      cur = cur->next;
      cnt++;
    }
    return cnt;
  }
  
  HoughLine* curLineList = lineList;
  HoughLine* curLine = lines;
  bool add = true;
  int numLines = 0;
  
  while (curLine != nullptr){
    while (curLineList != nullptr){
      if (this->mergeHoughLines(curLineList, curLine)){
        add = false;
        break;
      } else {
        curLineList = curLineList->next;
      }
    }
    
    if (add) {
      HoughLine* next = curLine->next;
      curLine->next = lineList;
      lineList = curLine;
      curLine = next;
      numLines++;
    } else {
      HoughLine* next = curLine->next;
      delete curLine;
      curLine = next;
    }
    
    add = true;
    curLineList = lineList;
  }
  
  return numLines;
}



bool OccupancyGrid::mergeHoughLines(HoughLine* line1, HoughLine* line2){
  double thetaDiff = (line1->theta) - (line2->theta);
  thetaDiff *= thetaDiff;
  double radiusDiff = (line1->radius) - (line2->radius);
  radiusDiff *= radiusDiff;
  
  int dx1, dy1, dx2, dy2;
  
  if ((thetaDiff < 0.0003046174198) && (radiusDiff < 2.0)){
    
    line1->theta = ((line1->theta) + (line2->theta))/2.0;
    line1->radius = ((line1->radius) + (line2->radius))/2.0;
    return true;
  
//     //check if close endpoints should be merged
//     dx1 = (line1->startX) - (line2->endX);
//     dy1 = (line1->startY) - (line2->endY);
//     
//     if ((dx1*dx1 + dy1*dy1) < BRIDGE) {
//       line1->startX = line2->startX;
//       line1->startY = line2->startY;
//       dx1 = line1->endX - line1->startX;
//       dy1 = line1->endY - line1->startY;
//       
//       line1->theta = atan2(dy1, dx1) + 1.570796327;
//       line1->radius = round((double)(line1->endX)*cos(line1->theta) + (double)(line1->endY)*sin(line1->theta));
//       return true;
//     }
//     
//     dx2 = (line2->startX) - (line1->endX);
//     dy2 = (line2->startY) - (line1->endY);
//     
//     if ((dx2*dx2 + dy2*dy2) < BRIDGE) {
//       line1->endX = line2->endX;
//       line1->endY = line2->endY;
//       dx2 = line1->endX - line1->startX;
//       dy2 = line1->endY - line1->startY;
//       
//       line1->theta = atan2(dy2, dx2) + 1.570796327;
//       line1->radius = round((double)(line1->endX)*cos(line1->theta) + (double)(line1->endY)*sin(line1->theta));
//       return true;
//     }
//     
//     //use dot products to see if lines overlap
//     dx1 = (line2->startX) - (line1->endX);
//     dy1 = (line2->startY) - (line1->endY);
//     
//     dx2 = (line1->endX) - (line1->startX);
//     dy2 = (line1->endY) - (line1->startY);
//     
//     if ((dx1*dx2 + dy1*dy2) >= 0) {
//       dx1 = (line1->startX) - (line2->endX);
//       dy1 = (line1->startY) - (line2->endY);
//     
//       dx2 = (line2->endX) - (line2->startX);
//       dy2 = (line2->endY) - (line2->startY);
//       
//       if ((dx1*dx2 + dy1*dy2) >= 0) {
//         //lines overlap, merge them
//         //TODO: merge the lines here with least squares
//         dx2 = line1->endX - line1->startX;
//         dy2 = line1->endY - line1->startY;
//         
//         line1->theta = atan2(dy2, dx2) + 1.570796327;
//         line1->radius = round((double)(line1->endX)*cos(line1->theta) + (double)(line1->endY)*sin(line1->theta));
//         return true;
//       }
//     }
  }
  
  return false;
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
