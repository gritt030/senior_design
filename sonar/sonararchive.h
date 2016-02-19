
#ifndef SONARARCHIVE_H
#define SONARARCHIVE_H

#include <cmath>

#include "sonarscan.h"
#include "./../occupancygrid/occupancygrid.h"

class SonarArchive
{
public:
  //percent of distance traveled that error is
  double POSITION_ERROR = 0.01;
  //percent of angles turned that error is
  double HEADING_ERROR = 0.01;
  
  static const int SCALE = 10;  //number of cm per grid square
  static const int SONAR_MAX = 325;    //maximum range on sonar (not based on scale)
  
  SonarArchive();
  ~SonarArchive();
  void addSonarScan(int* sonarDists, double x, double y, double xErr, double yErr, double head, double headErr);
  void removeSonarScan();
  OccupancyGrid* generateMap();
  OccupancyGrid* generateMapWalls();
  OccupancyGrid* generateMapSorted();
  OccupancyGrid* generateMapSortedNoBlur();
  OccupancyGrid* generateMapNoBlur();
  OccupancyGrid* generateMapReference();
  
  void sortScans();
  void sortScansX();
  void sortScansY();
  void sortXsortYScans();
  void reverseScans();
  void printScans();
  
private:
  SonarScan* prevScan = nullptr;
  void propagateXPosError();
  void propagateYPosError();
  void propagateHeadError();
  void getSonarCoords(SonarScan* scan, int* buffer);
  double getSonarAngles(SonarScan* scan);
};

#endif // SONARARCHIVE_H
