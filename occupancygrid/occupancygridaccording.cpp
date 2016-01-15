
#include "occupancygrid.h"

//Bresenham's Algorithm
//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
bool OccupancyGrid::openLineAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2){
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      x += signX;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<deltaX; i++) {
      if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      x += signX;
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    return true;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        if ((ref->grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) || (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD)) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
  return true;
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void OccupancyGrid::closeLineAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  if (this->openLineAccording(ref, relX1, relY1, relX2, relY2)) {
    //set final point of line to closed
    grid->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, -OccupancyGrid::OPEN);
    grid->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, OccupancyGrid::CLOSED);
  }
}


void OccupancyGrid::openSliceAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->openLineAccording(ref, relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openLineAccording(ref, relX1, relY1, relX2+i, relY2);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openLineAccording(ref, relX1, relY1, relX2, relY2+i);
    }
    return;
  }
  
  //get slope we're moving at
  float slope = -(float)deltaX/(float)deltaY;

  //for block size
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //amount we need to change x and y by to get ends of line
  int changeX = (int)round(sqrt((float)(dist*dist) / (1.0+slope*slope)));
  int changeY = (int)round((slope*(float)changeX));
  
  //starting and ending points of line
  int startX = relX2 - changeX;
  int startY = relY2 - changeY;
  int x = startX;
  int y = startY;
  
  //used to determine how to move
  int ySign;
  int block;
  
  //how many times we perform the loop for
  int loop;
  if (deltaX > deltaY){
    loop = abs(changeY)<<1;
    block = deltaX;
  }
  else {
    loop = changeX<<1;
    block = deltaY;
  }
  
  if (slope < 0) ySign = -1;
  else ySign = 1;
  
  //keep track of pixel transitions
  int cx = block>>1;
  int cy = block>>1;
    
  this->openLineAccording(ref, relX1, relY1, x, y);
  
  do {
    cx += deltaY;
    cy += deltaX;
    if (cx > block) {
      x++;
      cx -= block;
    }
    
    if (cy > block) {
      y += ySign;
      cy -= block;
    }
    
    //std::cout <<x<<","<<y<<std::endl;
    this->openLineAccording(ref, relX1, relY1, x, y);
  } while (loop-- > 0);
}


void OccupancyGrid::closeSliceAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->closeLineAccording(ref, relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeLineAccording(ref, relX1, relY1, relX2+i, relY2);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeLineAccording(ref, relX1, relY1, relX2, relY2+i);
    }
    return;
  }
  
  //get slope we're moving at
  float slope = -(float)deltaX/(float)deltaY;

  //for block size
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //amount we need to change x and y by to get ends of line
  int changeX = (int)round(sqrt((float)(dist*dist) / (1.0+slope*slope)));
  int changeY = (int)round((slope*(float)changeX));
  
  //starting and ending points of line
  int startX = relX2 - changeX;
  int startY = relY2 - changeY;
  int x = startX;
  int y = startY;
  
  //used to determine how to move
  int ySign;
  int block;
  
  //how many times we perform the loop for
  int loop;
  if (deltaX > deltaY){
    loop = abs(changeY)<<1;
    block = deltaX;
  }
  else {
    loop = changeX<<1;
    block = deltaY;
  }
  
  if (slope < 0) ySign = -1;
  else ySign = 1;
  
  //keep track of pixel transitions
  int cx = block>>1;
  int cy = block>>1;
    
  this->closeLineAccording(ref, relX1, relY1, x, y);
  
  do {
    cx += deltaY;
    cy += deltaX;
    if (cx > block) {
      x++;
      cx -= block;
    }
    
    if (cy > block) {
      y += ySign;
      cy -= block;
    }
    
    //std::cout <<x<<","<<y<<std::endl;
    this->closeLineAccording(ref, relX1, relY1, x, y);
  } while (loop-- > 0);
}

