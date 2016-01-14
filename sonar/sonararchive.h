
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
  
  static const int SONAR_MAX = 400;    //maximum range on sonar (not based on scale)
  
  SonarArchive();
  void addSonarScan(int* sonarDists, double x, double y, double xErr, double yErr, double head, double headErr);
  OccupancyGrid* generateMap();
  OccupancyGrid* generateMapWalls();
  OccupancyGrid* generateMapSorted();
  OccupancyGrid* generateMapSortedNoBlur();
  
private:
  SonarScan* prevScan = nullptr;
  void propagateXPosError();
  void propagateYPosError();
  void propagateHeadError();
  void getSonarCoords(SonarScan* scan, int* buffer);
  double getSonarAngles(SonarScan* scan);
  
  SonarScan* sortScans();
  SonarScan* sortScansX();
  SonarScan* sortScansY();
  
};

#endif // SONARARCHIVE_H
