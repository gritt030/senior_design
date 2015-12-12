
#include "resizableGrid.h"

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



/*
//Henry's Algorithm
//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void ResizableGrid::openNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
  //used to correct coordinates when setting values in grid
  int boundary = Node::GRID_SIZE / 2;
  
  //information on how we need to move
  int x = relX1;                //starting position
  int y = relY1;
  int deltaX = relX2 - relX1;   //distance we need to move
  int deltaY = relY2 - relY1;
  
  //special cases are handled here
  //single point
  if ((deltaX == 0) && (deltaY == 0)) {
    curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
    return;
    
  //vertical line
  } else if (deltaX == 0) {
    int signY = deltaY/abs(deltaY);
    
    for (int i=0; i<=abs(deltaY); i++) {
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      y += signY;
    }
    return;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<=abs(deltaX); i++) {
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      x += signX;
    }
    return;
  }
  
  //if we reach this point, we know line is diagonal
  int signX = deltaX/abs(deltaX);
  int signY = deltaY/abs(deltaY);
  
  //slope when moving in each direction
  float slopeX = (float)deltaX / (float)deltaY * (float)signY;
  float slopeY = (float)deltaY / (float)deltaX * (float)signX;
  
  //distance left to wall of current grid square
  float blockX = 0.5;
  float blockY = 0.5;
  
  //loop through all grid spaces on the line
  while ((x != relX2) || (y != relY2)) {
    //set value in grid to open
    curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
    
    //change in position to next block
    float chX = fabs(slopeX*blockY);
    float chY = fabs(slopeY*blockX);
    
    //passed through side wall
    if (blockX < chX) {
      x += signX;
      blockX = 1.0f;
      blockY -= chY;
      
    //passed through top wall
    } else if (blockY < chY) {
      y += signY;
      blockY = 1.0f;
      blockX -= chX;
      
    //passed through corner
    } else {
      x += signX;
      y += signY;
      blockX = 1.0f;
      blockY = 1.0f;
    } 
  }
  
  //set final point of line to open
  curNode->setValue(boundary+relX2, boundary-relY2, ResizableGrid::OPEN);
}   //*/







//Bresenham's Algorithm
//make all squares in a node grid between (x1,y1) and (x2,y2) open
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void ResizableGrid::openNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
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
    curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
    return;
    
  //vertical line
  } else if (deltaX == 0) {
    signY = deltaY/abs(deltaY);
    
    for (int i=0; i<=abs(deltaY); i++) {
      if (curNode->getValue(boundary+x, boundary-y) < 0) return;
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      y += signY;
    }
    return;
    
  //horizontal line
  } else if (deltaY == 0) {
    int signX = deltaX/abs(deltaX);
    
    for (int i=0; i<=abs(deltaX); i++) {
      if (curNode->getValue(boundary+x, boundary-y) < 0) return;
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      x += signX;
    }
    return;
  }
  
  //switch to absolute value coordinates to make things easier
  signX = deltaX/abs(deltaX);
  signY = deltaY/abs(deltaY);
  deltaX = abs(deltaX);
  deltaY = abs(deltaY);
    
  //SPECIAL CASE: diagonal line
  if (deltaY == deltaX) {
    for (int i=0; i<=deltaX; i++) {
      if (curNode->getValue(boundary+x, boundary-y) < 0) return;
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      x += signX;
      y += signY;
    }
    return;
  }
  
  //extra squares go in same direction as movement
  if ((signX == -1) && (signY == -1)) {
    x = relX2;
    y = relY2;
    signX = 1;
    signY = 1;
  }
  
  //change in x and y so far
  int cx=0, cy=0;
  
  //line more horizontal than vertical
  if (deltaX > deltaY) {
    for (int i=0; i<deltaX; i++) {
      //fill in current grid square
      if (curNode->getValue(boundary+x, boundary-y) < 0) return;
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      
      //see if we need to move vertically
      cy += deltaY;
      if (cy >= deltaX) {
        y += signY;
        cy -= deltaX;
        if (curNode->getValue(boundary+x, boundary-y) < 0) return;
        curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      }
      
      //move horizontally
      x += signX;
    }
    
  //line more vertical than horizontal
  } else {
    for (int i=0; i<deltaY; i++) {
      //fill in current grid square
      if (curNode->getValue(boundary+x, boundary-y) < 0) return;
      curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      
      //see if we need to move vertically
      cx += deltaX;
      if (cx >= deltaY) {
        x += signX;
        cx -= deltaY;
        if (curNode->getValue(boundary+x, boundary-y) < 0) return;
        curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
      }
      
      //move horizontally
      y += signY;
    }
  }
  
  if (curNode->getValue(boundary+x, boundary-y) < 0) return;
  curNode->setValue(boundary+x, boundary-y, ResizableGrid::OPEN);
}   //*/








//make all squares in a node grid between (x1,y1) and (x2,y2) open and close point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void ResizableGrid::closeNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
  //used to correct coordinates when setting values in grid
  int boundary = Node::GRID_SIZE / 2;
  
  //open line in grid
  ResizableGrid::openNodeLine(relX1, relY1, relX2, relY2, curNode);
  
  //set final point of line to closed
  curNode->setValue(boundary+relX2, boundary-relY2, ResizableGrid::CLOSED*10); //TODO: fix this fool!
}


//make all squares in a node grid between (x1,y1) and (x2,y2) open and frontier point (x2,y2)
// -GRID_SIZE/2 <= x1, x2, y1, y2 <= GRID_SIZE/2
// (0,0) is the central square in the grid
void ResizableGrid::frontierNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode){
  //used to correct coordinates when setting values in grid
  int boundary = Node::GRID_SIZE / 2;
  
  //open line in grid
  ResizableGrid::openNodeLine(relX1, relY1, relX2, relY2, curNode);
  
  //set final point of line to frontier
  curNode->setValue(boundary+relX2, boundary-relY2, ResizableGrid::FRONTIER);
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

