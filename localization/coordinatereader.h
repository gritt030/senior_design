
#ifndef COORDINATEREADER_H
#define COORDINATEREADER_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <algorithm>

class CoordinateReader
{
  public:
    CoordinateReader(char* filename);
    void getCurrentCoords(int* buffer);
    void goToNextCoords();
    
  private:
    std::ifstream coordFile;
    int* currentCoords;
};

#endif // COORDINATEREADER_H
