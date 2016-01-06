
#include "resizableGrid.h"

//gaussian kernels
const int ResizableGrid::KERNELS[][NUM_KERNELS + 1] = {{13,1,0,0,0,0,0,0,0,0},        //stdev = 0.33 decimeters
                                                       {45,18,1,0,0,0,0,0,0,0},       //stdev = 0.66
                                                       {64,40,10,1,0,0,0,0,0,0},      //stdev = 1
                                                       {74,56,25,7,1,0,0,0,0,0},      //stdev = 1.33
                                                       {79,66,39,16,5,1,0,0,0,0},     //stdev = 1.66
                                                       {82,73,50,27,12,4,1,0,0,0},    //stdev = 2
                                                       {84,77,59,37,20,9,3,1,0,0},    //stdev = 2.33
                                                       {85,80,65,46,28,15,7,3,1,0},   //stdev = 2.66
                                                       {86,82,69,53,36,22,12,6,3,1}}; //stdev = 3
 
const int ResizableGrid::KERNEL_SUMS[] = {15,83,166,252,333,416,496,575,654};


//constructor
ResizableGrid::ResizableGrid(int x, int y){
  //save position
  this->x = x;
  this->y = y;
  
  //create initial root node
  this->root = new Node();
}


//add a wall point to the global map
//position (x1, y1) is the starting point
//position (x2, y2) is the position of the closed space
//all spaces between the start and wall are marked as open
void ResizableGrid::addClosed(int x1, int y1, int x2, int y2){
}




//move the root of the map N, S, E, or W to the new position
//will only move one node at a time
void ResizableGrid::moveRoot(char dir){
  switch (dir) {
    case 'N':
      if (this->root->N != nullptr) {
        this->root = this->root->N;
        this->y += Node::GRID_SIZE;
      }
      break;
      
    case 'S':
      if (this->root->S != nullptr) {
        this->root = this->root->S;
        this->y -= Node::GRID_SIZE;
      }
      break;
      
    case 'E':
      if (this->root->E != nullptr) {
        this->root = this->root->E;
        this->x += Node::GRID_SIZE;
      }
      break;
      
    case 'W':
      if (this->root->W != nullptr) {
        this->root = this->root->W;
        this->x -= Node::GRID_SIZE;
      }
      break;
  }
}


//connects a new node to the current node in the given direction
//side should be N, S, E, W
void ResizableGrid::connectNewNode(Node* curNode, char side){
  //need search algorithm here
}
    

//get the node that contains the given position if it exists
//if node is not in the map, return nullptr
//uses A* to search the grid
Node* ResizableGrid::getNode(int x, int y){
  return nullptr;
}


Node* ResizableGrid::getNextNode(int relX, int relY, float dir, Node* curNode){
  return nullptr;
}


//Bresenham's Algorithm
//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
bool ResizableGrid::openNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
  //used to correct coordinates when setting values in grid
  int boundary = Node::GRID_SIZE / 2;
  
  //information on how we need to move
  int x = relX1;                //position variables
  int y = relY1;
  int signX, signY;             //sign of movement direction
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
    return true;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<abs(deltaY); i++) {
      if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
      curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      y += signY;
    }
    curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
    return true;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<abs(deltaX); i++) {
      if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
      curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      x += signX;
    }
    curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
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
      if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
      curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      x += signX;
      y += signY;
    }
    curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
    return true;
  }
  
  //change in x and y so far
  int cx=deltaY>>1, cy=deltaX>>1;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
      curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy > deltaX) {
        y += signY;
        cy -= deltaX;
        if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
        curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
      curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      
      //see if we need to move horizontally
      cx += deltaX;
      if (cx > deltaY) {
        x += signX;
        cx -= deltaY;
        if (curNode->getValue(boundary+x, boundary-y) < ResizableGrid::THRESHOLD) {curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN); return false;}
        curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      }
      
      //move vertically
      y += signY;
    }
  }
  
  //set last point in line
  curNode->changeValue(boundary+x, boundary-y, ResizableGrid::OPEN);
  return true;
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void ResizableGrid::closeNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
  //used to correct coordinates when setting values in grid
  int boundary = Node::GRID_SIZE / 2;
  
  //open line in grid
  if (ResizableGrid::openNodeLine(relX1, relY1, relX2, relY2, curNode)) {
    //set final point of line to closed
    curNode->changeValue(boundary+relX2, boundary-relY2, -ResizableGrid::OPEN);
    curNode->changeValue(boundary+relX2, boundary-relY2, ResizableGrid::CLOSED);
  }
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and frontier point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void ResizableGrid::frontierNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
  //used to correct coordinates when setting values in grid
  int boundary = Node::GRID_SIZE / 2;
  
  //open line in grid
  if (ResizableGrid::openNodeLine(relX1, relY1, relX2, relY2, curNode)) {
    //set final point in line to frontier
    curNode->setValue(boundary+relX2, boundary-relY2, ResizableGrid::FRONTIER);
  }
}


void ResizableGrid::sendToImage(char* filename){
  PngWriter* w = new PngWriter();
  int size = Node::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<Node::GRID_SIZE; i++){
    for (int j=0; j<Node::GRID_SIZE; j++) {
      char cur = this->root->getValue(i, j);
      setImagePixel(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
}


void ResizableGrid::setImagePixel(PngWriter* w, int x, int y, char value){
  int color;
  
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
  
  w->set_pixel(x, y, color);
}








void ResizableGrid::openSlice(int relX1, int relY1, int relX2, int relY2, float angle, Node* curNode){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->openNodeLine(relX1, relY1, relX2, relY2, curNode);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openNodeLine(relX1, relY1, relX2+i, relY2, curNode);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->openNodeLine(relX1, relY1, relX2, relY2+i, curNode);
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
  int endX = relX2 + changeX;
  int endY = relY2 + changeY;
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
    
  this->openNodeLine(relX1, relY1, x, y, curNode);
  
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
    this->openNodeLine(relX1, relY1, x, y, curNode);
  } while (loop-- > 0);
}


void ResizableGrid::closeSlice(int relX1, int relY1, int relX2, int relY2, float angle, Node* curNode){
  //information on how we need to move
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  int dist = (int)round(sqrt(deltaX*deltaX + deltaY*deltaY)*tan(angle));  //perpendicular distance along one leg
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    this->closeNodeLine(relX1, relY1, relX2, relY2, curNode);
    return;
    
  //vertical line passed in
  } else if (deltaX == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeNodeLine(relX1, relY1, relX2+i, relY2, curNode);
    }
    return;
    
  //horizontal line passed in
  } else if (deltaY == 0) {
    for (int i=-dist; i<=dist; i++) {
      this->closeNodeLine(relX1, relY1, relX2, relY2+i, curNode);
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
  int endX = relX2 + changeX;
  int endY = relY2 + changeY;
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
    
  this->closeNodeLine(relX1, relY1, x, y, curNode);
  
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
    this->closeNodeLine(relX1, relY1, x, y, curNode);
  } while (loop-- > 0);
}






//blur the map in the x direction
void ResizableGrid::blurMapX(int uncertainty, Node* curNode){
  //make sure uncertainty is not too small/large
  if (uncertainty < 1) return;
  else if (uncertainty > this->NUM_KERNELS) uncertainty = this->NUM_KERNELS;
  
  const int* kernel = this->KERNELS[uncertainty-1];
  const int kernelSum = this->KERNEL_SUMS[uncertainty-1];
  char* buffer = new char[uncertainty];
  
  for (int i=0; i<Node::GRID_SIZE; i++){
    for (int j=0; j<Node::GRID_SIZE+uncertainty; j++){
      //calculate current kernel sum
      int sum = 0;
      for (int k=uncertainty; k>0; k--){
        sum += (int)(curNode->getValue(j-k,i)) * kernel[k];
        sum += (int)(curNode->getValue(j+k,i)) * kernel[k];
      }
      sum += (int)(curNode->getValue(j,i)) * kernel[0];
      sum /= kernelSum;
      
      //set unneeded values to new values
      curNode->setValue(j-uncertainty,i,buffer[0]);
      
      //update buffer
      for (int k=1; k<uncertainty; k++){
        buffer[k-1] = buffer[k];
      }
      buffer[uncertainty-1] = (char)sum;
    }
  }
}


//blur the map in the y direction
void ResizableGrid::blurMapY(int uncertainty, Node* curNode){
  //make sure uncertainty is not too small/large
  if (uncertainty < 1) return;
  else if (uncertainty > this->NUM_KERNELS) uncertainty = this->NUM_KERNELS;
  
  const int* kernel = this->KERNELS[uncertainty-1];
  const int kernelSum = this->KERNEL_SUMS[uncertainty-1];
  char* buffer = new char[uncertainty];
  
  for (int i=0; i<Node::GRID_SIZE; i++){
    for (int j=0; j<Node::GRID_SIZE+uncertainty; j++){
      //calculate current kernel sum
      int sum = 0;
      for (int k=uncertainty; k>0; k--){
        sum += (int)(curNode->getValue(i,j-k)) * kernel[k];
        sum += (int)(curNode->getValue(i,j+k)) * kernel[k];
      }
      sum += (int)(curNode->getValue(i,j)) * kernel[0];
      sum /= kernelSum;
      
      //set unneeded values to new values
      curNode->setValue(i,j-uncertainty,buffer[0]);
      
      //update buffer
      for (int k=1; k<uncertainty; k++){
        buffer[k-1] = buffer[k];
      }
      buffer[uncertainty-1] = (char)sum;
    }
  }
}

