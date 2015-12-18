
#include "node.h"


//1 square per byte
//constructor, create grid
Node::Node() {
  this->map = new char[GRID_SIZE * GRID_SIZE]();
}


//set a value in the grid to a character
void Node::setValue(int x, int y, char value){
  //bounds checking TODO:Maybe get rid of this, takes 30ns
  if ((x<0) || (y<0) || (x>=Node::GRID_SIZE) || (y>=Node::GRID_SIZE)) {
    return;
  }
  
  //set value
  this->map[y*Node::GRID_SIZE + x] = value;
}


//change a value in the grid by the amount passed in
void Node::changeValue(int x, int y, char value){
  //bounds checking TODO:Maybe get rid of this, takes 30ns
  if ((x<0) || (y<0) || (x>=Node::GRID_SIZE) || (y>=Node::GRID_SIZE)) {
    return;
  }
  
  int index = y*Node::GRID_SIZE + x;
  char current = this->map[index];
  
  if (value > 0) {
    if (current <= (Node::MAX_VALUE - value)) {
      this->map[index] = value + current;
    }
  } else {
    if (current >= (-Node::MAX_VALUE-value)) {
      this->map[index] = value + current;
    }
  }
}


//get the value of a square in the grid
char Node::getValue(int x, int y){
  //bounds checking TODO:Maybe get rid of this, takes 30ns
  if ((x<0) || (y<0) || (x>=Node::GRID_SIZE) || (y>=Node::GRID_SIZE)) {
    return 0;
  }
  
  //return value
  char value = this->map[y*Node::GRID_SIZE + x];
  return value;
}



/*
//2 squares per byte
//constructor, create grid
Node::Node() {
  this->map = new char[(Node::GRID_SIZE * Node::GRID_SIZE + 1) >> 1]();
}


//set a value in the grid to certain 4-bit value (lsb's of value)
void Node::setValue(int x, int y, char value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Node::GRID_SIZE) || (y>=Node::GRID_SIZE)) {
    return;
  }
  
  //make sure value written is only 4 bits
  value &= 0x0f;
  
  int index = y*Node::GRID_SIZE + x;    //index in normal grid
  char shift = (index%2)*4;             //shift value by 4 if index is odd
  char ander = ~(0x0f << shift);        //AND current value to clear bits being set
  value <<= shift;                      //shift value by proper amount
  
  char cur = this->map[index >> 1];     //get current character
  cur &= ander;                         //clear bits being set
  cur |= value;                         //OR with new value
  this->map[index >> 1] = cur;          //set character in grid to new value
}
*/



/*
//4 squares per byte
//constructor, create grid
Node::Node() {
  this->map = new char[(Node::GRID_SIZE * Node::GRID_SIZE + 3) >> 2]();
}


//set a value in the grid to a certain 2-bit value (lsb's of value)
void Node::setValue(int x, int y, char value){
  //bounds checking
  if ((x<0) || (y<0) || (x>=Node::GRID_SIZE) || (y>=Node::GRID_SIZE)) {
    return;
  }
  
  //make sure value is only 2 bits
  value &= 0x03;
  
  int index = y*Node::GRID_SIZE + x;    //index in normal grid
  char shift = (index%4)*2;             //shift amount, low 2 bits are index 0, high 2 bits are index 3
  char ander = ~(0x03 << shift);        //AND value to clear bits being set
  value <<= shift;                      //shift value by proper amount
  
  char cur = this->map[index >> 2];     //get current value from grid
  cur &= ander;                         //AND value to clear bits being set
  cur |= value;                         //OR value to set bits
  this->map[index >> 2] = cur;          //set character in grid to new value
}
*/
