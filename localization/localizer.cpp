
#include "localizer.h"

//constructor
Localizer::Localizer(CoordinateReader* r){
  this->reader = r;
}


void Localizer::getPosition(int* buffer){
  int* coords = new int[2];
  this->getCoords(coords);
  
  buffer[0] = coords[0]/Localizer::SCALE;
  buffer[1] = coords[1]/Localizer::SCALE;
}


int Localizer::getPositionAccuracy(){
  int accuracy = this->getCoordAccuracy();
  return accuracy/Localizer::SCALE;
}


//TODO: make all of these apply rotation matrix to pos/distance combo
void Localizer::getWSonarPosition(int* buffer){
  int* coords = new int[2];
  this->getWSonarDist(coords);
  
  buffer[0] = coords[0]/Localizer::SCALE;
  buffer[1] = coords[1]/Localizer::SCALE;
}
void Localizer::getNWSonarPosition(int* buffer){
  int* coords = new int[2];
  this->getNWSonarDist(coords);
  
  buffer[0] = coords[0]/Localizer::SCALE;
  buffer[1] = coords[1]/Localizer::SCALE;
}
void Localizer::getNESonarPosition(int* buffer){
  int* coords = new int[2];
  this->getNESonarDist(coords);
  
  buffer[0] = coords[0]/Localizer::SCALE;
  buffer[1] = coords[1]/Localizer::SCALE;
}
void Localizer::getESonarPosition(int* buffer){
  int* coords = new int[2];
  this->getESonarDist(coords);
  
  buffer[0] = coords[0]/Localizer::SCALE;
  buffer[1] = coords[1]/Localizer::SCALE;
}


//TODO: fix all of this stuff
void Localizer::getCoords(int* buffer){
  int* all = new int[15];
  reader->getCurrentCoords(all);
  
  buffer[0] = all[0];
  buffer[1] = all[1];
}

int Localizer::getCoordAccuracy(){return 0;}

//TODO: make these return the distance measurements, not the coord values
void Localizer::getWSonarDist(int* buffer){
  int* all = new int[15];
  reader->getCurrentCoords(all);
  
  buffer[0] = all[9];
  buffer[1] = all[10];
}
void Localizer::getNWSonarDist(int* buffer){
  int* all = new int[15];
  reader->getCurrentCoords(all);
  
  buffer[0] = all[3];
  buffer[1] = all[4];
}
void Localizer::getNESonarDist(int* buffer){
  int* all = new int[15];
  reader->getCurrentCoords(all);
  
  buffer[0] = all[6];
  buffer[1] = all[7];
}
void Localizer::getESonarDist(int* buffer){
  int* all = new int[15];
  reader->getCurrentCoords(all);
  
  buffer[0] = all[12];
  buffer[1] = all[13];
}