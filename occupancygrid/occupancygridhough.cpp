
#include "occupancygrid.h"

HoughGrid* OccupancyGrid::performHoughTransform(){
  HoughGrid* h = new HoughGrid();
  
  //generate hough transform grid
  for (int i=0; i<Grid::GRID_SIZE; i++){
    for (int j=0; j<Grid::GRID_SIZE; j++){
      char cur = this->grid->getValue(i,j);
      if (cur < 0) h->addHoughPointWeighted(i,j,(unsigned short)(-cur));
    }
  }
  
  return h;
}


OccupancyGrid* OccupancyGrid::generateHoughMap(){
  //do hough transform
  HoughGrid* h = this->performHoughTransform();
  
  //generate new occupancy grid
  OccupancyGrid* g = new OccupancyGrid();
  
  //trace walls onto new occupancy grid
  this->traceHoughWalls(g, h);
  
  delete h;
  return g;
}

void OccupancyGrid::traceHoughWalls(OccupancyGrid* newGrid, HoughGrid* hough){
  hough->findMaxima();
    
  double theta;
  int radius;
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++){
      if (hough->getValue(i,j) != 0) {
        theta = 3.141592654*((double)i/(double)(HoughGrid::GRID_SIZE));
        radius = j - HoughGrid::ADDITION;
        this->traceHoughLine((double)radius, theta, newGrid);
      }
    }
  }
  
  this->traceCardinalDirections(hough->X_Cardinal, hough->Y_Cardinal, newGrid);
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
      //newGrid->grid->setValue((int)x, (int)y, -10); //TODO:remove
      
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
      //newGrid->grid->setValue((int)x, (int)y, -10); //TODO:remove
      
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
      setImagePixelHough(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
  delete h;
}


void OccupancyGrid::sendHoughMaximaToImage(char* filename){
  HoughGrid* h = this->performHoughTransform();
  h->findMaxima();
  
  PngWriter* w = new PngWriter();
  int size = HoughGrid::GRID_SIZE;
  
  w->create_image(filename, size, size);
  
  for (int i=0; i<HoughGrid::GRID_SIZE; i++){
    for (int j=0; j<HoughGrid::GRID_SIZE; j++) {
      unsigned short cur = h->getValue(i, j);
      setImagePixelHough(w, i, j, cur);
    }
  }
  
  w->output_image();
  delete w;
  delete h;
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
