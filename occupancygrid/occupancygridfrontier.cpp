
#include "occupancygrid.h"

bool OccupancyGrid::openLineFrontier(int relX1, int relY1, int relX2, int relY2){
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
    frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
    return true;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
    return true;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      x += signX;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
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
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      x += signX;
      y += signY;
    }
    grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
    frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
    return true;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
        frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
      grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
      frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        if (grid->getValue(BOUNDARY+x, BOUNDARY-y) < OccupancyGrid::THRESHOLD) {grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN); return false;}
        grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
        frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  grid->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::OPEN);
  frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
  return true;
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void OccupancyGrid::closeLineFrontier(int relX1, int relY1, int relX2, int relY2){
  //open line in grid
  if (this->openLineFrontier(relX1, relY1, relX2, relY2)) {
    //set final point of line to closed
    grid->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, -OccupancyGrid::OPEN);
    grid->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, OccupancyGrid::CLOSED);
    frontiers->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, -OccupancyGrid::FRONTIER);
    frontiers->changeValue(BOUNDARY+relX2, BOUNDARY-relY2, OccupancyGrid::UNFRONTIER);
  }
}


void OccupancyGrid::openSliceFrontier(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->openLineFrontier(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    this->openLineFrontier(relX1, relY1, relX2-dist, relY2);
    for (int i=-(dist-1); i<=(dist-1); i++) {
      if (this->openLine(relX1, relY1, relX2+i, relY2))
        frontiers->changeValue(BOUNDARY+relX2+i, BOUNDARY-relY2, OccupancyGrid::FRONTIER);
    }
    this->openLineFrontier(relX1, relY1, relX2+dist, relY2);
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    this->openLineFrontier(relX1, relY1, relX2, relY2-dist);
    for (int i=-(dist-1); i<=(dist-1); i++) {
      if (this->openLine(relX1, relY1, relX2, relY2+i))
        frontiers->changeValue(BOUNDARY+relX2, BOUNDARY-relY2-i, OccupancyGrid::FRONTIER);
    }
    this->openLineFrontier(relX1, relY1, relX2, relY2+dist);
    return;
  }
  
  //get slope we're moving at
  float slope = -(float)deltaX/(float)deltaY;

  //for block size
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //amount we need to change x and y by to get ends of line
  int changeX = (int)round(sqrt((float)(dist*dist) / (1.0+slope*slope)));
  int changeY = (int)round(sqrt((float)(dist*dist) / (1.0+1.0/(slope*slope))) * (slope/fabs(slope)));
  
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
    
  this->openLineFrontier(relX1, relY1, x, y);
  
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
    if (this->openLine(relX1, relY1, x, y))
      frontiers->changeValue(BOUNDARY+x, BOUNDARY-y, OccupancyGrid::FRONTIER);
  } while (loop-- > 1);
  
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
  
  this->openLineFrontier(relX1, relY1, x, y);
}


void OccupancyGrid::closeSliceFrontier(int relX1, int relY1, int relX2, int relY2, float angle){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->closeLineFrontier(relX1, relY1, relX2, relY2);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    this->closeLineFrontier(relX1, relY1, relX2-dist, relY2);
    for (int i=-(dist-1); i<=(dist-1); i++) {
      this->closeLine(relX1, relY1, relX2+i, relY2);
    }
    this->closeLineFrontier(relX1, relY1, relX2+dist, relY2);
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    this->closeLineFrontier(relX1, relY1, relX2, relY2-dist);
    for (int i=-(dist-1); i<=(dist-1); i++) {
      this->closeLine(relX1, relY1, relX2, relY2+i);
    }
    this->closeLineFrontier(relX1, relY1, relX2, relY2+dist);
    return;
  }
  //get slope we're moving at
  float slope = -(float)deltaX/(float)deltaY;

  //for block size
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
  
  //amount we need to change x and y by to get ends of line
  int changeX = (int)round(sqrt((float)(dist*dist) / (1.0+slope*slope)));
  int changeY = (int)round(sqrt((float)(dist*dist) / (1.0+1.0/(slope*slope))) * (slope/fabs(slope)));
  
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
  this->closeLineFrontier(relX1, relY1, x, y);
  
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
    this->closeLine(relX1, relY1, x, y);
  } while (loop-- > 1);

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
  
  this->closeLineFrontier(relX1, relY1, x, y);
}


void OccupancyGrid::sendToImageFrontier(char* filename){
  PngWriter* w = new PngWriter();
  int size = Grid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE; j++) {
      char curV = grid->getValue(i, j);
      char curF = frontiers->getValue(i, j);
      setImagePixelFrontier(w, i, j, curV, curF);
    }
  }
  
  w->output_image();
  delete w;
}


void OccupancyGrid::sendToImageSmall(char* filename){
  PngWriter* w = new PngWriter();
  int size = Grid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<Grid::GRID_SIZE/10; i++){
    for (int j=0; j<Grid::GRID_SIZE/10; j++) {
      int curV = 0;
      int curF = 0;
      for (int k=0; k<10; k++){
        for (int m=0; m<10; m++){
          curV += grid->getValue(10*i+k, 10*j+m);
          curF += frontiers->getValue(10*i+k, 10*j+m);
        }
      }
      if (curV > 127) curV = 127;
      if (curV < -127) curV = -127;
      //if (curV != 0) curV = 1;
      //curV /= 100;
      setImagePixelFrontier(w, i, j, curV, curF);
    }
  }
  
  w->output_image();
  delete w;
}


void OccupancyGrid::setImagePixelFrontier(PngWriter* w, int x, int y, char value, char front){
  int color;
  
  if (front > FRONT_THRESH) {
    color = (front<<17) | 0x000000ff;
  } else {
    //open map square
    if (value > 0) {
      color = (value<<17) | 0x0000ffff;
    
    //closed map square
    } else if (value < 0) {
      color = (value<<17) | 0xff0000ff;
    
    //unknown map square
    } else {
      color = 0x808080ff;
    }
  }
  
  w->set_pixel(x, y, color);
}



void OccupancyGrid::cleanFrontier(){
  Grid *newGrid = new Grid();
  
  //clear frontiers that are surrounded by closed cells
  for (int i=0; i<Grid::GRID_SIZE; i++) {
    for (int j=0; j<Grid::GRID_SIZE; j++) {
      if (frontiers->getValue(i,j) > FRONT_THRESH){
        int emptyMap = 0;
        if (grid->getValue(i+1,j) == 0) emptyMap++;
        if (grid->getValue(i-1,j) == 0) emptyMap++;
        if (grid->getValue(i,j+1) == 0) emptyMap++;
        if (grid->getValue(i,j-1) == 0) emptyMap++;
        
        if (emptyMap != 0)
          newGrid->setValue(i,j,frontiers->getValue(i,j));
      }
    }
  }
  
  delete this->frontiers;
  this->frontiers = newGrid;
  newGrid = new Grid();
  
  //clear frontiers that are alone
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE; j++) {
      if (frontiers->getValue(i,j) > FRONT_THRESH){
        int fullFron = 0;
        if (frontiers->getValue(i+1,j) > FRONT_THRESH) fullFron++;
        if (frontiers->getValue(i-1,j) > FRONT_THRESH) fullFron++;
        if (frontiers->getValue(i,j+1) > FRONT_THRESH) fullFron++;
        if (frontiers->getValue(i,j-1) > FRONT_THRESH) fullFron++;
        
        if (fullFron != 0)
          newGrid->setValue(i,j,frontiers->getValue(i,j));
      }
    }
  }
  
  delete this->frontiers;
  this->frontiers = newGrid;
}

