
#include "frontiermap.h"

FrontierMap::FrontierMap() {
  this->frontierGrid = new char[GRID_SIZE*GRID_SIZE];
}

FrontierMap::~FrontierMap() {
  delete this->frontierGrid;
}

