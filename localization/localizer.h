
#ifndef LOCALIZER_H
#define LOCALIZER_H

#include "coordinatereader.h"

class Localizer
{
  public:
    static const int SCALE = 10;    //cm per grid square
    
    //constructor
    Localizer(CoordinateReader* r);
    
    void getPosition(int* buffer);
    int getPositionAccuracy();
    
    void getWSonarPosition(int* buffer);
    void getNWSonarPosition(int* buffer);
    void getNESonarPosition(int* buffer);
    void getESonarPosition(int* buffer);
  
  private:
    CoordinateReader* reader;
    
    void getCoords(int* buffer);
    int getCoordAccuracy();
    void getWSonarDist(int* buffer);
    void getNWSonarDist(int* buffer);
    void getNESonarDist(int* buffer);
    void getESonarDist(int* buffer);
    
};

#endif // LOCALIZER_H
