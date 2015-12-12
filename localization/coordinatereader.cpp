
#include "coordinatereader.h"

CoordinateReader::CoordinateReader(char* filename){
  this->coordFile.open(filename);
  this->currentCoords = new int[15];
  
  for (int i=0; i<15; i++){
    currentCoords[i] = 0;
  }
}

void CoordinateReader::getCurrentCoords(int* buffer){
  for (int i=0; i<15; i++){
    buffer[i] = currentCoords[i];
  }
}


void CoordinateReader::goToNextCoords(){  
  long time;
  double x, y, z;
  double nwx, nwy, nwz;
  double nex, ney, nez;
  double wx, wy, wz;
  double ex, ey, ez;
  
  coordFile >> time;
  coordFile >> x >> y >> z;
  coordFile >> nwx >> nwy >> nwz;
  coordFile >> nex >> ney >> nez;
  coordFile >> wx >> wy >> wz;
  coordFile >> ex >> ey >> ez;
  
  if (coordFile.eof()){
    for (int i=0; i<15; i++){
      currentCoords[i] = 0;
    }
    return;
  }
  
  double scale = 10.0;
  
  this->currentCoords[0] = round(x/scale);
  this->currentCoords[1] = round(y/scale);
  this->currentCoords[2] = round(z/scale);
  this->currentCoords[3] = round(nwx/scale);
  this->currentCoords[4] = round(nwy/scale);
  this->currentCoords[5] = round(nwz/scale);
  this->currentCoords[6] = round(nex/scale);
  this->currentCoords[7] = round(ney/scale);
  this->currentCoords[8] = round(nez/scale);
  this->currentCoords[9] = round(wx/scale);
  this->currentCoords[10] = round(wy/scale);
  this->currentCoords[11] = round(wz/scale);
  this->currentCoords[12] = round(ex/scale);
  this->currentCoords[13] = round(ey/scale);
  this->currentCoords[14] = round(ez/scale);
  /*
  std::cout << "New Pos: " << currentCoords[0] << ", ";
  std::cout << currentCoords[0] << ", " << currentCoords[1] << ", ";
  std::cout << currentCoords[3] << ", " << currentCoords[4] << ", ";
  std::cout << currentCoords[6] << ", " << currentCoords[7] << ", ";
  std::cout << currentCoords[9] << ", " << currentCoords[10] << ", ";
  std::cout << currentCoords[12] << ", " << currentCoords[13] << std::endl;
  //*/
}
