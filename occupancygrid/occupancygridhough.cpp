
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
  
  this->X_Cardinal = s->X_Cardinal;
  this->Y_Cardinal = s->Y_Cardinal;
  
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
  
  int comLines = 0;
  int mrgLines = 0;
  
  int comb;
  
  char* name = new char[128];
  int ind = 0;
  int angle, distance;
  
  std::cout << "X_Card: " << this->X_Cardinal;
  std::cout << ", Y_Card: " << this->Y_Cardinal << std::endl;
  std::cout << "Finding lines:\n";
  
  for (int t=0; t<HoughGrid::GRID_SIZE; t++) {
    for (int r=0; r<HoughGrid::GRID_SIZE; r++) {
      index = t + r*HoughGrid::GRID_SIZE;
      if (houghLS->lsqrGrid->map[index] > 0) {
        theta = 3.141592654*((double)t/(double)(HoughGrid::GRID_SIZE));
        radius = r - HoughGrid::ADDITION;
        
        curLines = this->getHoughLines(radius, theta);
        if (curLines != nullptr) {
          
          
          comb = this->combineHoughLines(curLines);
          if (comb == 0) continue;
          
          
          
//           HoughLine* curLine = curLines;
//           int count = 0;
// //           newGrid = new OccupancyGrid();
// //           angle = (int)round(theta * 57.29577951);
// //           distance = (int)round(radius);
// //           if (distance < 0){
// //             angle = (angle + 180)%360;
// //             distance *= -1;
// //           }
// //           std::sprintf(name, "/home/owner/pics/pics/sequence/p_%04d_%05d_%d.png", angle, distance, ind++);
//           while (curLine != nullptr){
//             this->drawHoughLine(curLine, newGrid);
//             
// // //             int dx = curLine->endX - curLine->startX;
// // //             int dy = curLine->endY - curLine->startY;
// // //             int dist = dx*dx + dy*dy;
// // //             std::cout << dist << std::endl;
//             
//             std::cout << curLine->radius << ",";
//             std::cout << curLine->theta << ",";
//             std::cout << curLine->startX << ",";
//             std::cout << curLine->startY << ",";
//             std::cout << curLine->endX << ",";
//             std::cout << curLine->endY << "\n";
//             curLine = curLine->next;
//             count++;
//           }
// //           newGrid->sendToImage(name, 0,0);
// //           delete newGrid;
// //           std::cout << count << std::endl;
//           continue;
          
          
          
          comLines += comb;
          mrgLines += this->mergeLineList(curLines);
          
          
          
//           HoughLine* curLine = lineList;
//           int count = 0;
//           while (curLine != nullptr){
//             std::cout << curLine->radius << ",";
//             std::cout << curLine->theta << ",";
//             std::cout << curLine->startX << ",";
//             std::cout << curLine->startY << ",";
//             std::cout << curLine->endX << ",";
//             std::cout << curLine->endY << "\n";
//             curLine = curLine->next;
//             count++;
//           }
//           std::cout << count << std::endl;
//           continue;
          
          
          
          
          
          
          
          
          
          
//           int cur = 0;
//           HoughLine* curl = lineList;
//           while (curl != nullptr){
//             cur++;
//             curl = curl->next;
//           }
//           std::cout << totLines << "," << merLines << "," << cur << std::endl;
        }
      }
    }
  }
  
  std::cout << "Combined lines: " << comLines << std::endl;
  std::cout << "Merged Lines: " << mrgLines << std::endl;
  
  std::cout << "Number of lines: ";
  std::cout << collapseLineList() << std::endl;
  
  int line = 0;
  HoughLine* curLine = lineList;
  while (curLine != nullptr){
    this->drawHoughLine(curLine, newGrid);
    
            std::cout << curLine->radius << ",";
            std::cout << curLine->theta << ",";
            std::cout << curLine->startX << ",";
            std::cout << curLine->startY << ",";
            std::cout << curLine->endX << ",";
            std::cout << curLine->endY << "\n";
    
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
  
//   //line more horizontal than vertical
//   if ((theta > 0.7853981634) && (theta < 2.35619449)) {
//     for (x=0; x<Grid::GRID_SIZE; x++){
//       y = (radius-(x-HoughGrid::CENTER)*c)/s + HoughGrid::CENTER;
//       newGrid->grid->setValue((int)x, (int)y, -10);
//     }
//   
//   //line more vertical than horizontal
//   } else {
//     for (y=0; y<Grid::GRID_SIZE; y++){
//       x = (radius-(y-HoughGrid::CENTER)*s)/c + HoughGrid::CENTER;
//       newGrid->grid->setValue((int)x, (int)y, -10);
//     }
//   }

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
          startX = x;// - HoughGrid::CENTER;
          startY = y;// - HoughGrid::CENTER;
          endX = x;// - HoughGrid::CENTER;
          endY = y;// - HoughGrid::CENTER;
          newLine = false;
        } else {
          endX = x;// - HoughGrid::CENTER;
          endY = y;// - HoughGrid::CENTER;
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
          startX = x;// - HoughGrid::CENTER;
          startY = y;// - HoughGrid::CENTER;
          endX = x;// - HoughGrid::CENTER;
          endY = y;// - HoughGrid::CENTER;
          newLine = false;
        } else {
          endX = x;// - HoughGrid::CENTER;
          endY = y;// - HoughGrid::CENTER;
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
//       numLines--;
    }    
  }
  
  
  
  
  
  
//   prevLine = lines;
//   curLine = lines->next;
//   numLines = 0;
//     
//   while (curLine != nullptr){
//     xDiff = (curLine->startX) - (prevLine->endX);
//     yDiff = (curLine->startY) - (prevLine->endY);
//     diff = xDiff*xDiff + yDiff*yDiff;
//         
//     //if this line and preline are close, make into one line
//     if (diff < 1000){
//       prevLine->endX = curLine->endX;
//       prevLine->endY = curLine->endY;
//       prevLine->next = curLine->next;
//       delete curLine;
//       curLine = prevLine->next;
//     
//     //make sure prevline is long enough to keep
//     } else {
//       numLines++;
//       prevLine = curLine;
//       curLine = curLine->next;
//     }
//   }
  
  
  
//   curLine = lines;
//   int startX = curLine->startX;
//   int startY = curLine->startY;
//   int endX = curLine->endX;
//   int endY = curLine->endY;
//   
//   while (curLine != nullptr){
//     endX = curLine->endX;
//     endY = curLine->endY;
//     curLine = curLine->next;
//   }
//   
//   lines->startX = startX;
//   lines->startY = startY;
//   lines->endX = endX;
//   lines->endY = endY;
//   lines->next = nullptr;
//   return 1;
  
  
  
  
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








int OccupancyGrid::collapseLineList(){
  if (lineList == nullptr){
    return 0;
  }
  
  HoughLine* newLineList;
  HoughLine* lines;
  bool stillMerging = true;
  int numLines;
  
  while (stillMerging) {
    newLineList = this->lineList;
    lines = this->lineList->next;
    newLineList->next = nullptr;
  
    HoughLine* curLineList = newLineList;
    HoughLine* curLine = lines;
    bool add = true;
    stillMerging = false;
    numLines = 0;
    
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
        curLine->next = newLineList;
        newLineList = curLine;
        curLine = next;
        numLines++;
      } else {
        HoughLine* next = curLine->next;
        delete curLine;
        curLine = next;
        stillMerging = true;
      }
      
      add = true;
      curLineList = newLineList;
    }
    
    this->lineList = newLineList;
  }
  
  return numLines;
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
  double diffYcard1 = (line1->theta) - (this->Y_Cardinal);
  double diffYcard2 = (line2->theta) - (this->Y_Cardinal);
  double diffXcard1 = (line1->theta) - (this->X_Cardinal);
  double diffXcard2 = (line2->theta) - (this->X_Cardinal);
  
  diffYcard1 = fabs(diffYcard1);
  diffYcard2 = fabs(diffYcard2);
  diffXcard1 = fabs(diffXcard1);
  diffXcard2 = fabs(diffXcard2);
  
  if (diffYcard1 > 1.570796327) diffYcard1 -= 3.141592654;
  if (diffYcard2 > 1.570796327) diffYcard2 -= 3.141592654;
  if (diffXcard1 > 1.570796327) diffXcard1 -= 3.141592654;
  if (diffXcard2 > 1.570796327) diffXcard2 -= 3.141592654;
  
  diffYcard1 = fabs(diffYcard1);
  diffYcard2 = fabs(diffYcard2);
  diffXcard1 = fabs(diffXcard1);
  diffXcard2 = fabs(diffXcard2);
  
  double threshY = 0.05;
  double threshX = 0.06;
  double radThresh = 10.0;
  
  if ((diffYcard1 > threshY) && (diffXcard1 > threshX)){
    if ((diffYcard2 > threshY) && (diffXcard2 > threshX)){
      //lines are both too far from a cardinal direction, drop both
      line1->theta = 15.70796327;
      line1->radius = 0;
      line1->startX = 0;
      line1->startY = 0;
      line1->endX = 0;
      line1->endY = 0;
      return true;
    } else {
      //line 1 is too far from cardinal direction, drop it
      line1->theta = line2->theta;;
      line1->radius = line2->radius;
      line1->startX = line2->startX;
      line1->startY = line2->startY;
      line1->endX = line2->endX;
      line1->endY = line2->endY;
      return true;
    }
  } else if ((diffYcard2 > threshY) && (diffXcard2 > threshX)){
    //line 2 is too far from a cardinal direction, drop it
    return true;
  }
  
  //lines are both close to a cardinal direction, try to combine
  
  if ((diffYcard1 < threshY) && (diffYcard2 < threshY)){
    double rad1 = line1->radius;
    double rad2 = line2->radius;
    
    diffYcard1 = (line1->theta) - (this->Y_Cardinal);
    diffYcard2 = (line2->theta) - (this->Y_Cardinal);
    diffYcard1 = fabs(diffYcard1);
    diffYcard2 = fabs(diffYcard2);
    if (diffYcard1 > 1.570796327) rad1 = -1.0*rad1;
    if (diffYcard2 > 1.570796327) rad2 = -1.0*rad2;
    
    double radDiff = rad1 - rad2;
    radDiff = fabs(radDiff);
    
    //compare radius and merge, vertical case
    if (radDiff < radThresh){
      //merge lines
//       std::cout << line1->radius << ", ";
//       std::cout << line2->radius << ", ";
//       std::cout << (rad1 +rad2)/2.0;
//       std::cout << std::endl;
      
//       std::cout << line1->radius << ", ";
//       std::cout << line1->theta << ", ";
//       std::cout << line1->startX << ", ";
//       std::cout << line1->startY << ", ";
//       std::cout << line1->endX << ", ";
//       std::cout << line1->endY << ", ";
//       std::cout << line2->radius << ", ";
//       std::cout << line2->theta << ", ";
//       std::cout << line2->startX << ", ";
//       std::cout << line2->startY << ", ";
//       std::cout << line2->endX << ", ";
//       std::cout << line2->endY << ", ";
      
//       int dd1 = abs((line1->endY) - (line2->startY));
//       int dd2 = abs((line2->endY) - (line1->startY));
//       if ((dd1 > 10) && (dd2 > 10)) return false;
      
      
      double dist1 = fabs((line1->endY) - (line1->startY));
      double dist2 = fabs((line2->endY) - (line2->startY));
      
      line1->theta = this->Y_Cardinal;
      line1->radius = (rad1*dist1 + rad2*dist2)/(dist1 + dist2);
      
      int s1 = line1->startY;
      int s2 = line2->startY;
      int e1 = line1->endY;
      int e2 = line2->endY;
      
      int d1 = abs(s1 - e1);
      int d2 = abs(s1 - e2);
      int d3 = abs(s2 - e1);
      int d4 = abs(s2 - e2);
      
      if ((d2 > d1) && (d2 > d3) && (d2 > d4)){
        line1->endX = line2->endX;
        line1->endY = line2->endY;
      } else if ((d3 > d1) && (d3 > d4)){
        line1->startX = line2->startX;
        line1->startY = line2->startY;
      } else if (d4 > d1){
        line1->startX = line2->startX;
        line1->startY = line2->startY;
        line1->endX = line2->endX;
        line1->endY = line2->endY;
      }
      
//       std::cout << line1->radius << ", ";
//       std::cout << line1->theta << ", ";
//       std::cout << line1->startX << ", ";
//       std::cout << line1->startY << ", ";
//       std::cout << line1->endX << ", ";
//       std::cout << line1->endY << "\n";
      
      return true;
    } else {
//       std::cout << line1->radius << ", " << line2->radius << ", ";
//       std::cout << line1->theta << ", " << line2->theta << ", ";
//       std::cout << line1->startX << ", " << line2->startX << ", ";
//       std::cout << line1->startY << ", " << line2->startY << ", ";
//       std::cout << line1->endX << ", " << line2->endX << ", ";
//       std::cout << line1->endY << ", " << line2->endY << "\n";
    }
    
  } else if ((diffXcard1 < threshX) && (diffXcard2 < threshX)){
    double rad1 = line1->radius;
    double rad2 = line2->radius;
    
    diffXcard1 = (line1->theta) - (this->X_Cardinal);
    diffXcard2 = (line2->theta) - (this->X_Cardinal);
    diffXcard1 = fabs(diffXcard1);
    diffXcard2 = fabs(diffXcard2);
    if (diffXcard1 > 1.570796327) rad1 = -1.0*rad1;
    if (diffXcard2 > 1.570796327) rad2 = -1.0*rad2;
    
    double radDiff = rad1 - rad2;
    radDiff = fabs(radDiff);
    
    //compare radius and merge, horizontal case
    if (radDiff < radThresh){
      double dist1 = fabs((line1->endX) - (line1->startX));
      double dist2 = fabs((line2->endX) - (line2->startX));
      
      //merge lines
      line1->theta = this->X_Cardinal;
      line1->radius = (rad1*dist1 + rad2*dist2)/(dist1 + dist2);
      
      int s1 = line1->startX;
      int s2 = line2->startX;
      int e1 = line1->endX;
      int e2 = line2->endX;
      
      int d1 = abs(s1 - e1);
      int d2 = abs(s1 - e2);
      int d3 = abs(s2 - e1);
      int d4 = abs(s2 - e2);
      
      if ((d2 > d1) && (d2 > d3) && (d2 > d4)){
        line1->endX = line2->endX;
        line1->endY = line2->endY;
      } else if ((d3 > d1) && (d3 > d4)){
        line1->startX = line2->startX;
        line1->startY = line2->startY;
      } else if (d4 > d1){
        line1->startX = line2->startX;
        line1->startY = line2->startY;
        line1->endX = line2->endX;
        line1->endY = line2->endY;
      }
      
      return true;
    } else {
//       std::cout << line1->radius << ", " << line2->radius << ", ";
//       std::cout << line1->theta << ", " << line2->theta << ", ";
//       std::cout << line1->startX << ", " << line2->startX << ", ";
//       std::cout << line1->startY << ", " << line2->startY << ", ";
//       std::cout << line1->endX << ", " << line2->endX << ", ";
//       std::cout << line1->endY << ", " << line2->endY << "\n";
    }
  }
  
  //lines cannot be merged
  return false;
  
  
  
  
  
  double thresh = -1.0;
  if (diffYcard1 < thresh) {
    if (diffYcard2 < thresh) {
      return false;
    } else {
      return true;
    }
  } else {
    if (diffYcard2 < thresh) {
      line1->theta = line2->theta;;
      line1->radius = line2->radius;
      line1->startX = line2->startX;
      line1->startY = line2->startY;
      line1->endX = line2->endX;
      line1->endY = line2->endY;
      return true;
    } else {
      line1->theta = 0;
      line1->radius = 0;
      line1->startX = 0;
      line1->startY = 0;
      line1->endX = 0;
      line1->endY = 0;
      return true;
    }
  }
  
  return false;
  
  int diff12x = (line1->endX) - (line2->startX);
  int diff12y = (line1->endY) - (line2->startY);
  int diff12 = diff12x*diff12x + diff12y*diff12y;
  
  int diff21x = (line2->endX) - (line1->startX);
  int diff21y = (line2->endY) - (line1->startY);
  int diff21 = diff12x*diff12x + diff12y*diff12y;
  
  double thetaDiff = (line1->theta) - (line2->theta);
  thetaDiff *= thetaDiff;
  double radiusDiff = (line1->radius) - (line2->radius);
  radiusDiff *= radiusDiff;
  
  int joinThresh = 10;
  joinThresh *= joinThresh;
  int newBRIDGE = 5;
  newBRIDGE *= newBRIDGE;
  
  int dx1, dy1, dx2, dy2;
  double c, s, c1, s1, c2, s2;
  double diff1, diff2;
  
  double det1, det2;
  double intX, intY;
  double newTheta, newRadius;
  int newSX, newEX, newSY, newEY;
  
  /*
  if (diff12 > joinThresh){
    //check if close endpoints should be merged
    if (diff12 > newBRIDGE) {
      //use dot products to see if lines overlap
      dx1 = (line2->startX) - (line1->endX);
      dy1 = (line2->startY) - (line1->endY);
      dx2 = (line1->startX) - (line1->endX);
      dy2 = (line1->startY) - (line1->endY);
      
      if ((dx1*dx2 + dy1*dy2) < 0) {
        dx1 = (line1->startX) - (line2->endX);
        dy1 = (line1->startY) - (line2->endY);
        dx2 = (line2->startX) - (line2->endX);
        dy2 = (line2->startY) - (line2->endY);
        
        if ((dx1*dx2 + dy1*dy2) < 0) {
          //lines don't overlap, don't merge them
          std::cout << "No merge: ";
          std::cout << line1->radius << ", " << line2->radius << ", ";
          std::cout << line1->theta << ", " << line2->theta << ", ";
          std::cout << line1->startX << ", " << line2->startX << ", ";
          std::cout << line1->startY << ", " << line2->startY << ", ";
          std::cout << line1->endX << ", " << line2->endX << ", ";
          std::cout << line1->endY << ", " << line2->endY << "\n";
          return false;
        }
      }
    }
  } else if (diff21 > joinThresh){
    //check if close endpoints should be merged
    if (diff21 > newBRIDGE) {
      //use dot products to see if lines overlap
      dx1 = (line2->startX) - (line1->endX);
      dy1 = (line2->startY) - (line1->endY);
      dx2 = (line1->startX) - (line1->endX);
      dy2 = (line1->startY) - (line1->endY);
      
      if ((dx1*dx2 + dy1*dy2) < 0) {
        dx1 = (line1->startX) - (line2->endX);
        dy1 = (line1->startY) - (line2->endY);
        dx2 = (line2->startX) - (line2->endX);
        dy2 = (line2->startY) - (line2->endY);
        
        if ((dx1*dx2 + dy1*dy2) < 0) {
          //lines don't overlap, don't merge them
          std::cout << "No merge: ";
          std::cout << line1->radius << ", " << line2->radius << ", ";
          std::cout << line1->theta << ", " << line2->theta << ", ";
          std::cout << line1->startX << ", " << line2->startX << ", ";
          std::cout << line1->startY << ", " << line2->startY << ", ";
          std::cout << line1->endX << ", " << line2->endX << ", ";
          std::cout << line1->endY << ", " << line2->endY << "\n";
          return false;
        }
      }
    }
  }
  */
  
  
  if ((diff12 > joinThresh) && (diff21 > joinThresh)) return false;
  
  dx1 = (line1->endX) - (line1->startX);
  dy1 = (line1->endY) - (line1->startY);
  diff1 = sqrt(dx1*dx1 + dy1*dy1);    //weights
  
  dx2 = (line2->endX) - (line2->startX);
  dy2 = (line2->endY) - (line2->startY);
  diff2 = sqrt(dx2*dx2 + dy2*dy2);    //weights
  
  newTheta = (diff1*(line1->theta) + diff2*(line2->theta))/(diff1+diff2);
  
  c = cos(newTheta);        //angles
  s = sin(newTheta);
  c1 = cos(line1->theta);     
  s1 = sin(line1->theta);
  c2 = cos(line2->theta);
  s2 = sin(line2->theta);
  
  det1 = c*s1 - s*c1;       //determinants
  det2 = c*s2 - s*c2;
  
  if (det1 == 0.0) {
    
  }
  
  double det = -1.0;
    
    if (det == 0.0) {
      newTheta = line1->theta;
      newRadius = ((line1->radius) + (line2->radius))/2.0;
    } else {
      newTheta = ((diff1*line1->theta) + (diff2*line2->theta))/(diff1 + diff2);
//       newTheta = ((diff1*line1->radius) + (diff2*line2->radius))/(diff1 + diff2);
      
      intX = ((s2*line1->radius) - (s1*line2->radius))/det;
      intY = ((c1*line2->radius) - (c2*line1->radius))/det;
      newRadius = intX*cos(newTheta) + intY*sin(newTheta);
    }
    
    double interTheta = newTheta + 1.570796327;
    double startRad1 = (line1->startX)*cos(interTheta) + (line1->startY)*sin(interTheta);
    double startRad2 = (line2->startX)*cos(interTheta) + (line2->startY)*sin(interTheta);
    double endRad1 = (line1->endX)*cos(interTheta) + (line1->endY)*sin(interTheta);
    double endRad2 = (line2->endX)*cos(interTheta) + (line2->endY)*sin(interTheta);
    
    double interDet = cos(newTheta)*sin(interTheta) - sin(newTheta)*cos(interTheta);
    
    double interSX1 = (newRadius*sin(interTheta) - startRad1*sin(newTheta))/interDet;
    double interSX2 = (newRadius*sin(interTheta) - startRad2*sin(newTheta))/interDet;
    double interEX1 = (newRadius*sin(interTheta) - endRad1*sin(newTheta))/interDet;
    double interEX2 = (newRadius*sin(interTheta) - endRad2*sin(newTheta))/interDet;
    
    double interSY1 = (startRad1*cos(newTheta) - newRadius*cos(interTheta))/interDet;
    double interSY2 = (startRad2*cos(newTheta) - newRadius*cos(interTheta))/interDet;
    double interEY1 = (endRad1*cos(newTheta) - newRadius*cos(interTheta))/interDet;
    double interEY2 = (endRad2*cos(newTheta) - newRadius*cos(interTheta))/interDet;
    
    //1,1
    double len1 = (interSX1 - interEX1)*(interSX1 - interEX1) + (interSY1 - interEY1)*(interSY1 - interEY1);
    //1,2
    double len2 = (interSX1 - interEX2)*(interSX1 - interEX2) + (interSY1 - interEY2)*(interSY1 - interEY2);
    //2,1
    double len3 = (interSX2 - interEX1)*(interSX2 - interEX1) + (interSY2 - interEY1)*(interSY2 - interEY1);
    //2,2
    double len4 = (interSX2 - interEX2)*(interSX2 - interEX2) + (interSY2 - interEY2)*(interSY2 - interEY2);
    
    if ((len1 > len2) && (len1 > len3) && (len1 > len4)){
      newSX = (int)round(interSX1);
      newSY = (int)round(interSY1);
      newEX = (int)round(interEX1);
      newEY = (int)round(interEY1);
    } else if ((len2 > len3) && (len2 > len4)){
      newSX = (int)round(interSX1);
      newSY = (int)round(interSY1);
      newEX = (int)round(interEX2);
      newEY = (int)round(interEY2);
    } else if (len3 > len4){
      newSX = (int)round(interSX2);
      newSY = (int)round(interSY2);
      newEX = (int)round(interEX1);
      newEY = (int)round(interEY1);
    } else {
      newSX = (int)round(interSX2);
      newSY = (int)round(interSY2);
      newEX = (int)round(interEX2);
      newEY = (int)round(interEY2);
    }
    
    //std::cout << (line1->radius) << ", " << (line2->radius) << ", " << (line1->theta) << ", " << (line2->theta) << std::endl;
    
    line1->theta = ((line1->theta) + (line2->theta))/2.0;
    line1->radius = ((line1->radius) + (line2->radius))/2.0;
    
    line1->theta = (diff1*(line1->theta) + diff2*(line2->theta))/(diff1+diff2);
    line1->radius = (diff1*(line1->radius) + diff2*(line2->radius))/(diff1+diff2);
    
    newSX = ((line1->startX) + (line2->startX))/2;
    newSY = ((line1->startY) + (line2->startY))/2;
    newEX = ((line1->endX) + (line2->endX))/2;
    newEY = ((line1->endY) + (line2->endY))/2;

//     line1->theta = newTheta;
//     line1->radius = newRadius;
    line1->startX = newSX;
    line1->startY = newSY;
    line1->endX = newEX;
    line1->endY = newEY;
    return true;
  }







// bool OccupancyGrid::mergeHoughLines(HoughLine* line1, HoughLine* line2){ //return false;///////
//   double thetaDiff = (line1->theta) - (line2->theta);
//   thetaDiff *= thetaDiff;
//   double radiusDiff = (line1->radius) - (line2->radius);
//   radiusDiff *= radiusDiff;
//   
//   int dx1, dy1, dx2, dy2;
//   double c1, s1, c2, s2;
//   double diff1, diff2;
//   
//   double det;
//   double intX, intY;
//   double newTheta, newRadius;
//   int newSX, newEX, newSY, newEY;
//   
//   double thetaThresh = 30.0*0.0174532925;
//   double radiusThresh = 10.0;
//   
//   if ((thetaDiff < (thetaThresh * thetaThresh)) && (radiusDiff < (radiusThresh*radiusThresh))){
//     
//     //check if close endpoints should be merged
//     dx1 = (line1->startX) - (line2->endX);
//     dy1 = (line1->startY) - (line2->endY);
//     
//     if ((dx1*dx1 + dy1*dy1) > BRIDGE) {
//       dx2 = (line2->startX) - (line1->endX);
//       dy2 = (line2->startY) - (line1->endY);
//       
//       if ((dx2*dx2 + dy2*dy2) > BRIDGE) {
//         //use dot products to see if lines overlap
//         dx1 = (line2->startX) - (line1->endX);
//         dy1 = (line2->startY) - (line1->endY);
//         
// //         dx2 = (line1->endX) - (line1->startX);
// //         dy2 = (line1->endY) - (line1->startY);
//         dx2 = (line1->startX) - (line1->endX);
//         dy2 = (line1->startY) - (line1->endY);
//         
//         if ((dx1*dx2 + dy1*dy2) < 0) {
//           dx1 = (line1->startX) - (line2->endX);
//           dy1 = (line1->startY) - (line2->endY);
//         
// //           dx2 = (line2->endX) - (line2->startX);
// //           dy2 = (line2->endY) - (line2->startY);
//           dx2 = (line2->startX) - (line2->endX);
//           dy2 = (line2->startY) - (line2->endY);
//           
//           if ((dx1*dx2 + dy1*dy2) < 0) {
//             //lines don't overlap, don't merge them
//             std::cout << line1->radius << ", " << line2->radius << ", ";
//             std::cout << line1->theta << ", " << line2->theta << ", ";
//             std::cout << line1->startX << ", " << line2->startX << ", ";
//             std::cout << line1->startY << ", " << line2->startY << ", ";
//             std::cout << line1->endX << ", " << line2->endX << ", ";
//             std::cout << line1->endY << ", " << line2->endY << "\n";
//             return false;
//           }
//         }
//       }
//     }
//     
//     
//     
//     
//     
//     
//     
//     dx1 = (line1->endX) - (line1->startX);
//     dy1 = (line1->endY) - (line1->startY);
//     diff1 = sqrt(dx1*dx1 + dy1*dy1);    //weights
//     
//     dx2 = (line2->endX) - (line2->startX);
//     dy2 = (line2->endY) - (line2->startY);
//     diff2 = sqrt(dx2*dx2 + dy2*dy2);    //weights
//     
//     c1 = cos(line1->theta);     //angles
//     s1 = sin(line1->theta);
//     c2 = cos(line2->theta);
//     s2 = sin(line2->theta);
//     
//     det = c1*s2 - s1*c2;        //determinant
//     
//     if (det == 0.0) {
//       newTheta = line1->theta;
//       newRadius = ((line1->radius) + (line2->radius))/2.0;
//     } else {
//       newTheta = ((diff1*line1->theta) + (diff2*line2->theta))/(diff1 + diff2);
// //       newTheta = ((diff1*line1->radius) + (diff2*line2->radius))/(diff1 + diff2);
//       
//       intX = ((s2*line1->radius) - (s1*line2->radius))/det;
//       intY = ((c1*line2->radius) - (c2*line1->radius))/det;
//       newRadius = intX*cos(newTheta) + intY*sin(newTheta);
//     }
//     
//     double interTheta = newTheta + 1.570796327;
//     double startRad1 = (line1->startX)*cos(interTheta) + (line1->startY)*sin(interTheta);
//     double startRad2 = (line2->startX)*cos(interTheta) + (line2->startY)*sin(interTheta);
//     double endRad1 = (line1->endX)*cos(interTheta) + (line1->endY)*sin(interTheta);
//     double endRad2 = (line2->endX)*cos(interTheta) + (line2->endY)*sin(interTheta);
//     
//     double interDet = cos(newTheta)*sin(interTheta) - sin(newTheta)*cos(interTheta);
//     
//     double interSX1 = (newRadius*sin(interTheta) - startRad1*sin(newTheta))/interDet;
//     double interSX2 = (newRadius*sin(interTheta) - startRad2*sin(newTheta))/interDet;
//     double interEX1 = (newRadius*sin(interTheta) - endRad1*sin(newTheta))/interDet;
//     double interEX2 = (newRadius*sin(interTheta) - endRad2*sin(newTheta))/interDet;
//     
//     double interSY1 = (startRad1*cos(newTheta) - newRadius*cos(interTheta))/interDet;
//     double interSY2 = (startRad2*cos(newTheta) - newRadius*cos(interTheta))/interDet;
//     double interEY1 = (endRad1*cos(newTheta) - newRadius*cos(interTheta))/interDet;
//     double interEY2 = (endRad2*cos(newTheta) - newRadius*cos(interTheta))/interDet;
//     
//     //1,1
//     double len1 = (interSX1 - interEX1)*(interSX1 - interEX1) + (interSY1 - interEY1)*(interSY1 - interEY1);
//     //1,2
//     double len2 = (interSX1 - interEX2)*(interSX1 - interEX2) + (interSY1 - interEY2)*(interSY1 - interEY2);
//     //2,1
//     double len3 = (interSX2 - interEX1)*(interSX2 - interEX1) + (interSY2 - interEY1)*(interSY2 - interEY1);
//     //2,2
//     double len4 = (interSX2 - interEX2)*(interSX2 - interEX2) + (interSY2 - interEY2)*(interSY2 - interEY2);
//     
//     if ((len1 > len2) && (len1 > len3) && (len1 > len4)){
//       newSX = (int)round(interSX1);
//       newSY = (int)round(interSY1);
//       newEX = (int)round(interEX1);
//       newEY = (int)round(interEY1);
//     } else if ((len2 > len3) && (len2 > len4)){
//       newSX = (int)round(interSX1);
//       newSY = (int)round(interSY1);
//       newEX = (int)round(interEX2);
//       newEY = (int)round(interEY2);
//     } else if (len3 > len4){
//       newSX = (int)round(interSX2);
//       newSY = (int)round(interSY2);
//       newEX = (int)round(interEX1);
//       newEY = (int)round(interEY1);
//     } else {
//       newSX = (int)round(interSX2);
//       newSY = (int)round(interSY2);
//       newEX = (int)round(interEX2);
//       newEY = (int)round(interEY2);
//     }
//     
//     //std::cout << (line1->radius) << ", " << (line2->radius) << ", " << (line1->theta) << ", " << (line2->theta) << std::endl;
//     
//     line1->theta = ((line1->theta) + (line2->theta))/2.0;
//     line1->radius = ((line1->radius) + (line2->radius))/2.0;
//     
//     line1->theta = (diff1*(line1->theta) + diff2*(line2->theta))/(diff1+diff2);
//     line1->radius = (diff1*(line1->radius) + diff2*(line2->radius))/(diff1+diff2);
//     
//     newSX = ((line1->startX) + (line2->startX))/2;
//     newSY = ((line1->startY) + (line2->startY))/2;
//     newEX = ((line1->endX) + (line2->endX))/2;
//     newEY = ((line1->endY) + (line2->endY))/2;
// 
// //     line1->theta = newTheta;
// //     line1->radius = newRadius;
//     line1->startX = newSX;
//     line1->startY = newSY;
//     line1->endX = newEX;
//     line1->endY = newEY;
//     return true;
//   }
//   
//   return false;
// }











void OccupancyGrid::traceCardinalDirections(double x_card, double y_card, OccupancyGrid* newGrid){
  return;
  
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
      setImagePixelHough(w, i, j, cur*500);
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
  
//   s->performLeastSquares();
//   
//   for (int i=0; i<HoughGrid::GRID_SIZE; i++){
//     for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
//       unsigned short cur = s->lsqrGrid->getValue(i, j);
//       if (cur > 0) setImagePixelHough(w, i, j, 65535);
//     }
//   }
  
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
