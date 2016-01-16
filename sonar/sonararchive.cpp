
#include "sonararchive.h"
#define HALF_PI 1.570796327

//constructor
SonarArchive::SonarArchive(){}


void SonarArchive::addSonarScan(int* sonarDists, double x, double y, double xErr, double yErr, double head, double headErr){
  SonarScan* scan = new SonarScan();
  scan->w = sonarDists[0];
  scan->nw = sonarDists[1];
  scan->ne = sonarDists[2];
  scan->e = sonarDists[3];
  scan->x = x;
  scan->y = y;
  scan->xErr = xErr;
  scan->yErr = yErr;
  scan->heading = head;
  scan->headErr = headErr;
  
  scan->previous = this->prevScan;
  this->prevScan = scan;
  
  if (scan->previous == nullptr) return;
  if (scan->previous->xErr < scan->xErr) this->propagateXPosError();
  if (scan->previous->yErr < scan->yErr) this->propagateYPosError();
  if (scan->previous->headErr < scan->headErr) this->propagateHeadError();
}


void SonarArchive::propagateXPosError(){
  double xError = this->prevScan->xErr;
  double prevX = this->prevScan->x;
  
  SonarScan* curScan = this->prevScan->previous;
  if (curScan == nullptr) return;
  double xDist = abs(curScan->x - prevX);
  double newErr = xError + POSITION_ERROR*xDist;
  
  while (newErr < curScan->xErr) {
    curScan->xErr = newErr;
    prevX = curScan->x;
    
    curScan = curScan->previous;
    if (curScan == nullptr) return;
    
    xDist += abs(curScan->x - prevX);
    newErr = xError + POSITION_ERROR*xDist;
  }
}


void SonarArchive::propagateYPosError(){
  double yError = this->prevScan->yErr;
  double prevY = this->prevScan->y;
  
  SonarScan* curScan = this->prevScan->previous;
  if (curScan == nullptr) return;
  double yDist = abs(curScan->y - prevY);
  double newErr = yError + POSITION_ERROR*yDist;
  
  while (newErr < curScan->yErr) {
    curScan->yErr = newErr;
    prevY = curScan->y;
    
    curScan = curScan->previous;
    if (curScan == nullptr) return;
    
    yDist += abs(curScan->y - prevY);
    newErr = yError + POSITION_ERROR*yDist;
  }
}


void SonarArchive::propagateHeadError(){
  double hError = this->prevScan->headErr;
  double prevH = this->prevScan->heading;
  
  SonarScan* curScan = this->prevScan->previous;
  if (curScan == nullptr) return;
  double hDist = abs(curScan->heading - prevH);
  double newErr = hError + HEADING_ERROR*hDist;
  
  while (newErr < curScan->headErr) {
    curScan->headErr = newErr;
    prevH = curScan->heading;
    
    curScan = curScan->previous;
    if (curScan == nullptr) return;
    
    hDist += abs(curScan->heading - prevH);
    newErr = hError + HEADING_ERROR*hDist;
  }
}


OccupancyGrid* SonarArchive::generateMap(){
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  double angle;
  int curBlurX;
  int curBlurY;
  int prevBlurX = 0;
  int prevBlurY = 0;
  
  /*
  char* let = new char[5];
  let[0] = '0';
  let[1] = '1';
  let[2] = '2';
  let[3] = '3';
  let[4] = '4';
  */
  
  //don't need
  int i=0;
  
  SonarScan* current = this->prevScan;
  OccupancyGrid* localMap = new OccupancyGrid();
  
  while (current != nullptr){
    i++;
    x = (int)(current->x/SCALE);
    y = (int)(current->y/SCALE);
    curBlurX = (int)(current->xErr/SCALE);
    curBlurY = (int)(current->yErr/SCALE);
    
    if ((curBlurX != prevBlurX) || (curBlurY != prevBlurY)){
      localMap->blurMapX(prevBlurX);
      localMap->blurMapY(prevBlurY);
      output->mergeMaps(localMap);
      
      /*
      char* file = new char[128];
      strcpy(file, "/home/owner/pics/large/blurLocal");
      file[32] = let[(i/25)%5];
      file[33] = let[(i/5)%5];
      file[34] = let[i%5];
      file[35] = '\0';
      strcat(file, ".png");
      localMap->sendToImage(file);
      */
      
      
      delete localMap;
      localMap = new OccupancyGrid();
      prevBlurX = curBlurX;
      prevBlurY = curBlurY;
      //don't need
      std::cout << "Merging " << i << "..." << std::endl;
    }
    
    this->getSonarCoords(current, buf);
    angle = this->getSonarAngles(current);
    
     if (current->w < SONAR_MAX) localMap->closeSlice(x, y, x+buf[0], y+buf[1], (float)angle);
     else localMap->openSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    
    if (current->nw < SONAR_MAX) localMap->closeSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    else localMap->openSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    
     if (current->ne < SONAR_MAX) localMap->closeSlice(x, y, x+buf[4], y+buf[5], (float)angle);
     else localMap->openSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    
     if (current->e < SONAR_MAX) localMap->closeSlice(x, y, x+buf[6], y+buf[7], (float)angle);
     else localMap->openSlice(x, y, x+buf[6], y+buf[7], (float)angle);

    current = current->previous;
  }
  
  output->mergeMaps(localMap);
  delete localMap;
  return output;
}




OccupancyGrid* SonarArchive::generateMapWalls(){
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  double angle;
  int curBlurX;
  int curBlurY;
  int prevBlurX = 0;
  int prevBlurY = 0;
  
  //don't need
  int i=0;
  
  SonarScan* current = this->prevScan;
  OccupancyGrid* localMap = new OccupancyGrid();
  
  while (current != nullptr){
    i++;
    x = (int)(current->x/SCALE);
    y = (int)(current->y/SCALE);
    curBlurX = (int)(current->xErr/SCALE);
    curBlurY = (int)(current->yErr/SCALE);
    
    if ((curBlurX != prevBlurX) || (curBlurY != prevBlurY)){
      localMap->blurMapX(prevBlurX);
      localMap->blurMapY(prevBlurY);
      output->mergeMaps(localMap);
      
      delete localMap;
      localMap = new OccupancyGrid();
      prevBlurX = curBlurX;
      prevBlurY = curBlurY;
      //don't need
      std::cout << "Merging " << i << "..." << std::endl;
    }
    
    this->getSonarCoords(current, buf);
    angle = this->getSonarAngles(current);
    
    if (current->w < SONAR_MAX) localMap->closeSlice(x+buf[0], y+buf[1], x+buf[0], y+buf[1], (float)angle);
    
    if (current->nw < SONAR_MAX) localMap->closeSlice(x+buf[2], y+buf[3], x+buf[2], y+buf[3], (float)angle);
    
    if (current->ne < SONAR_MAX) localMap->closeSlice(x+buf[4], y+buf[5], x+buf[4], y+buf[5], (float)angle);
    
    if (current->e < SONAR_MAX) localMap->closeSlice(x+buf[6], y+buf[7], x+buf[6], y+buf[7], (float)angle);

    current = current->previous;
  }
  
  output->mergeMaps(localMap);
  delete localMap;
  return output;
}


OccupancyGrid* SonarArchive::generateMapSorted(){
  std::cout << "Sorting..." << std::endl;
  this->sortXsortYScans();
  
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  double angle;
  int curBlurX;
  int curBlurY;
  int prevBlurX = 0;
  int prevBlurY = 0;
  
  /*
  char* let = new char[5];
  let[0] = '0';
  let[1] = '1';
  let[2] = '2';
  let[3] = '3';
  let[4] = '4'; //*/
  
  //don't need
  int i=0;
  
  SonarScan* current = this->prevScan;
  OccupancyGrid* localMap = new OccupancyGrid();
  OccupancyGrid* tempMap = new OccupancyGrid();
  
  while (current != nullptr){
    i++;
    x = (int)(current->x/SCALE);
    y = (int)(current->y/SCALE);
    curBlurX = (int)(current->xErr/SCALE);
    curBlurY = (int)(current->yErr/SCALE);
    
    if (curBlurY != prevBlurY){
      tempMap->blurMapY(prevBlurY);
      localMap->mergeMaps(tempMap);
      
      delete tempMap;
      tempMap = new OccupancyGrid();
      prevBlurY = curBlurY;
      //don't need
      std::cout << "Merging Y" << i << "..." << std::endl;
    }
    
    if (curBlurX != prevBlurX){
      tempMap->blurMapY(prevBlurY);
      localMap->mergeMaps(tempMap);
      delete tempMap;
      tempMap = new OccupancyGrid();
      
      localMap->blurMapX(prevBlurX);
      output->mergeMaps(localMap);
      
      /*
      char* file = new char[128];
      strcpy(file, "/home/owner/pics/large/blurLocal");
      file[32] = let[(i/25)%5];
      file[33] = let[(i/5)%5];
      file[34] = let[i%5];
      file[35] = '\0';
      strcat(file, ".png");
      localMap->sendToImage(file); //*/
      
      delete localMap;
      localMap = new OccupancyGrid();
      prevBlurX = curBlurX;
      //don't need
      std::cout << "Merging X" << i << "..." << curBlurX << std::endl;
    }
    
    this->getSonarCoords(current, buf);
    angle = this->getSonarAngles(current);
    
    if (current->w < SONAR_MAX) tempMap->closeSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    else tempMap->openSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    
    if (current->nw < SONAR_MAX) tempMap->closeSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    else tempMap->openSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    
    if (current->ne < SONAR_MAX) tempMap->closeSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    else tempMap->openSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    
    if (current->e < SONAR_MAX) tempMap->closeSlice(x, y, x+buf[6], y+buf[7], (float)angle);
    else tempMap->openSlice(x, y, x+buf[6], y+buf[7], (float)angle);

    current = current->previous;
  }
  
  tempMap->blurMapY(prevBlurY);
  localMap->mergeMaps(tempMap);
  delete tempMap;
  
  localMap->blurMapX(prevBlurX);
  output->mergeMaps(localMap);
  delete localMap;
  
  return output;
}








OccupancyGrid* SonarArchive::generateMapReference(){
  this->sortScans();
  
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  double angle;
  int curBlurX;
  int curBlurY;
  int prevBlurX = 0;
  int prevBlurY = 0;
  
  /*
  char* let = new char[5];
  let[0] = '0';
  let[1] = '1';
  let[2] = '2';
  let[3] = '3';
  let[4] = '4';
  */
  
  //don't need
  int i=0;
  
  SonarScan* current = this->prevScan;
  OccupancyGrid* localMap = new OccupancyGrid();
  
  while (current != nullptr){
    i++;
    x = (int)(current->x/SCALE);
    y = (int)(current->y/SCALE);
    curBlurX = (int)(current->xErr/SCALE);
    curBlurY = (int)(current->yErr/SCALE);
    
    if ((curBlurX != prevBlurX) || (curBlurY != prevBlurY)){
      localMap->blurMapX(prevBlurX);
      localMap->blurMapY(prevBlurY);
      output->mergeMaps(localMap);
      
      /*
      char* file = new char[128];
      strcpy(file, "/home/owner/pics/large/blurLocal");
      file[32] = let[(i/25)%5];
      file[33] = let[(i/5)%5];
      file[34] = let[i%5];
      file[35] = '\0';
      strcat(file, ".png");
      localMap->sendToImage(file);
      */
      
      
      delete localMap;
      localMap = new OccupancyGrid();
      prevBlurX = curBlurX;
      prevBlurY = curBlurY;
      //don't need
      std::cout << "Merging " << i << "..." << std::endl;
    }
    
    this->getSonarCoords(current, buf);
    angle = this->getSonarAngles(current);
    
    if (current->w < SONAR_MAX) localMap->closeSliceAccording(output, x, y, x+buf[0], y+buf[1], (float)angle);
    else localMap->openSliceAccording(output, x, y, x+buf[0], y+buf[1], (float)angle);
    
    if (current->nw < SONAR_MAX) localMap->closeSliceAccording(output, x, y, x+buf[2], y+buf[3], (float)angle);
    else localMap->openSliceAccording(output, x, y, x+buf[2], y+buf[3], (float)angle);
    
    if (current->ne < SONAR_MAX) localMap->closeSliceAccording(output, x, y, x+buf[4], y+buf[5], (float)angle);
    else localMap->openSliceAccording(output, x, y, x+buf[4], y+buf[5], (float)angle);
    
    if (current->e < SONAR_MAX) localMap->closeSliceAccording(output, x, y, x+buf[6], y+buf[7], (float)angle);
    else localMap->openSliceAccording(output, x, y, x+buf[6], y+buf[7], (float)angle);

    current = current->previous;
  }
  
  output->mergeMaps(localMap);
  delete localMap;
  return output;
}


OccupancyGrid* SonarArchive::generateMapNoBlur(){
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  double angle;
  
  SonarScan* current = this->prevScan;
  
  while (current != nullptr){  
    x = (int)(current->x/SCALE);
    y = (int)(current->y/SCALE);
    
    this->getSonarCoords(current, buf);
    angle = this->getSonarAngles(current);
    
    if (current->w < SONAR_MAX) output->closeSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    else output->openSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    
    if (current->nw < SONAR_MAX) output->closeSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    else output->openSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    
    if (current->ne < SONAR_MAX) output->closeSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    else output->openSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    
    if (current->e < SONAR_MAX) output->closeSlice(x, y, x+buf[6], y+buf[7], (float)angle);
    else output->openSlice(x, y, x+buf[6], y+buf[7], (float)angle);

    current = current->previous;
  }

  return output;
}


OccupancyGrid* SonarArchive::generateMapSortedNoBlur(){
  this->sortScans();
  
  OccupancyGrid* output = new OccupancyGrid();
  int* buf = new int[8];
  int x, y;
  double angle;
  
  SonarScan* current = this->prevScan;
  
  while (current != nullptr){  
    x = (int)(current->x/SCALE);
    y = (int)(current->y/SCALE);
    
    this->getSonarCoords(current, buf);
    angle = this->getSonarAngles(current);
    
    if (current->w < SONAR_MAX) output->closeSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    else output->openSlice(x, y, x+buf[0], y+buf[1], (float)angle);
    
    if (current->nw < SONAR_MAX) output->closeSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    else output->openSlice(x, y, x+buf[2], y+buf[3], (float)angle);
    
    if (current->ne < SONAR_MAX) output->closeSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    else output->openSlice(x, y, x+buf[4], y+buf[5], (float)angle);
    
    if (current->e < SONAR_MAX) output->closeSlice(x, y, x+buf[6], y+buf[7], (float)angle);
    else output->openSlice(x, y, x+buf[6], y+buf[7], (float)angle);

    current = current->previous;
  }

  return output;
}



void SonarArchive::getSonarCoords(SonarScan* scan, int* buffer){
  double Angle = scan->heading;
  
  double x = (double)scan->w + 10.0;
  double y = 0.0;
  buffer[0] = (int)round(x*cos(Angle+HALF_PI) - y*sin(Angle+HALF_PI));
  buffer[1] = (int)round(x*sin(Angle+HALF_PI) + y*cos(Angle+HALF_PI));
  
  x = (double)scan->nw + 10.0;
  y = 10.0;
  buffer[2] = (int)round(x*cos(Angle) - y*sin(Angle));
  buffer[3] = (int)round(x*sin(Angle) + y*cos(Angle));
  
  x = (double)scan->ne + 10.0;
  y = -10.0;
  buffer[4] = (int)round(x*cos(Angle) - y*sin(Angle));
  buffer[5] = (int)round(x*sin(Angle) + y*cos(Angle));
  
  x = (double)scan->e + 10.0;
  y = 0.0;
  buffer[6] = (int)round(x*cos(Angle-HALF_PI) - y*sin(Angle-HALF_PI));
  buffer[7] = (int)round(x*sin(Angle-HALF_PI) + y*cos(Angle-HALF_PI));
  
  buffer[0] /= SCALE;
  buffer[1] /= SCALE;
  buffer[2] /= SCALE;
  buffer[3] /= SCALE;
  buffer[4] /= SCALE;
  buffer[5] /= SCALE;
  buffer[6] /= SCALE;
  buffer[7] /= SCALE;
}


double SonarArchive::getSonarAngles(SonarScan* scan){
  return scan->headErr;
}



void SonarArchive::sortScans(){
  SonarScan* first = this->prevScan;
  this->prevScan = first->previous;
  first->previous = nullptr;
  
  SonarScan* c = this->prevScan;
  
  while (c != nullptr){
    this->prevScan = c->previous;
    c->previous = nullptr;
    
    double curX = c->xErr;
    double curY = c->yErr;
    double curErr = sqrt(curX*curX + curY*curY);
    
    SonarScan* p = nullptr;
    SonarScan* s = first;
    
    while (s != nullptr){
      double newX = s->xErr;
      double newY = s->yErr;
      double newErr = sqrt(newX*newX + newY*newY);
      
      if (curErr < newErr){
        if (p == nullptr){
          c->previous = first;
          first = c;
          break;
        } else {
          c->previous = s;
          p->previous = c;
          break;
        }
      } else {
        p = s;
        s = s->previous;
      }
    }
    
    if (s==nullptr) {
      p->previous = c;
    }
    
    c = this->prevScan;
  }
  
  this->prevScan = first;
}

void SonarArchive::sortScansX(){
  SonarScan* first = this->prevScan;
  this->prevScan = first->previous;
  first->previous = nullptr;
  
  SonarScan* c = this->prevScan;
  
  while (c != nullptr){
    this->prevScan = c->previous;
    c->previous = nullptr;
    
    double curErr = c->xErr;
    
    SonarScan* p = nullptr;
    SonarScan* s = first;
    
    while (s != nullptr){
      double newErr = s->xErr;
      
      if (curErr < newErr){
        if (p == nullptr){
          c->previous = first;
          first = c;
          break;
        } else {
          c->previous = s;
          p->previous = c;
          break;
        }
      } else {
        p = s;
        s = s->previous;
      }
    }
    
    if (s==nullptr) {
      p->previous = c;
    }
    
    c = this->prevScan;
  }
  
  this->prevScan = first;
}

void SonarArchive::sortScansY(){
  SonarScan* first = this->prevScan;
  this->prevScan = first->previous;
  first->previous = nullptr;
  
  SonarScan* c = this->prevScan;
  
  while (c != nullptr){
    this->prevScan = c->previous;
    c->previous = nullptr;
    
    double curErr = c->yErr;
    
    SonarScan* p = nullptr;
    SonarScan* s = first;
    
    while (s != nullptr){
      double newErr = s->yErr;
      
      if (curErr < newErr){
        if (p == nullptr){
          c->previous = first;
          first = c;
          break;
        } else {
          c->previous = s;
          p->previous = c;
          break;
        }
      } else {
        p = s;
        s = s->previous;
      }
    }
    
    if (s==nullptr) {
      p->previous = c;
    }
    
    c = this->prevScan;
  }
  
  this->prevScan = first;
}

void SonarArchive::sortXsortYScans(){
  SonarScan* mainHead = nullptr, *mainTail;
  this->sortScansX();
  
  SonarScan* curChain = this->prevScan;
  
  while (curChain != nullptr) {
    int curBlur = (int)(curChain->xErr/SCALE);
    
    SonarScan* curTail = curChain;
    SonarScan* cur = curChain->previous;
    while (cur != nullptr) {
      if ((int)(cur->xErr/SCALE) != curBlur){
        break;
      }
      curTail = cur;
      cur = cur->previous;
    }

    this->prevScan = cur;
    curTail->previous = nullptr;
    
    SonarScan* newChain = curChain;
    curChain = newChain->previous;
    newChain->previous = nullptr;
    
    SonarScan* c = curChain;
    
    while (c != nullptr){
      curChain = c->previous;
      c->previous = nullptr;
      
      double curErr = c->yErr;
      
      SonarScan* p = nullptr;
      SonarScan* s = newChain;
      
      while (s != nullptr){
        double newErr = s->yErr;
        
        if (curErr < newErr){
          if (p == nullptr){
            c->previous = newChain;
            newChain = c;
            break;
          } else {
            c->previous = s;
            p->previous = c;
            break;
          }
        } else {
          p = s;
          s = s->previous;
        }
      }
      
      if (s==nullptr) {
        p->previous = c;
      }
      
      c = curChain;
    }
    
    if (mainHead == nullptr){
      mainHead = newChain;
    } else {
      mainTail->previous = newChain;
    }
    
    while (newChain->previous != nullptr){
      newChain = newChain->previous;
    }
    
    mainTail = newChain;
    curChain = this->prevScan;
  }
  
  this->prevScan = mainHead;
}




void SonarArchive::printScans() {
  SonarScan* cur = this->prevScan;
  
  while (cur != nullptr) {
    std::cout << cur->xErr << ", " << cur->yErr << std::endl;
    cur = cur->previous;
  }
}













