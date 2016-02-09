#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <cstdio>
#include <ctime>


#include "occupancygrid/grid/grid.h"
#include "occupancygrid/occupancygrid.h"
#include "localization/coordinatereader.h"
#include "localization/localizer.h"
#include "navigation/virtualenvironment.h"
#include "sonar/sonarscan.h"
#include "sonar/sonararchive.h"
#include "navigation/navigationmap/navigationmap.h"
#include "occupancygrid/grid/houghgrid.h"

#include "pngwriter/png_writer.h"


int main(int argc, char **argv) {
  if (argc != 2) {std::cout << "More args please!\n"; return 0;}
  
  char* occImg = "/home/owner/pics/pics/occupancy.png";
  char* sorImg = "/home/owner/pics/pics/refined.png";
  char* rawImg = "/home/owner/pics/pics/rawnav.png";
  char* navImg = "/home/owner/pics/pics/navigate.png";
  //char* coordFile = "/home/owner/workspace/Datasets/output_ds3/coordsEstimate.txt";
  char* coordFile = (char*)argv[1];
  
  VirtualEnvironment* v = new VirtualEnvironment();
  v->setPosition(100,2500);
  v->setRotation(0.0f);
  int* buf1 = new int[15];
  v->getCurrentCoordinates(buf1);
  
  CoordinateReader* r = new CoordinateReader(coordFile);
  
  r->updateCoordsFile();  //use file for maps
  
  /*//use virtual environment for maps
  r->updateCoordsVirtual(buf1);
  //*/
  
  Localizer* l = new Localizer(r);
  
  NavigationMap* g = new NavigationMap();
  //OccupancyGrid* g = new OccupancyGrid();
  
  SonarArchive* a = new SonarArchive();
  
  int* drone = new int[3];
  int* weson = new int[3];
  int* eason = new int[3];
  int* nwson = new int[3];
  int* neson = new int[3];
  bool* range = new bool[4];
  float angle = 0.17;
  double heading = 0.0;
  double distX = 0.0;
  double distY = 0.0;
  double distA = 0.0;
  double* prevLoc = new double[5];
  double prevAngle = 0.0;
  int* sonarDists = new int[4];
  double* rawPos = new double[5];
  
  double XDrift = 0.0;
  double YDrift = 0.0;
  
  int index = 0;
  char* name = new char[128];
  
  std::srand(std::time(nullptr));
  int num = 0;
  
  /*
  std::cout << "here we go...\n";
  OccupancyGrid* gr = new OccupancyGrid();
  gr->closeLine(0,0,0,0);
  gr->closeLine(5,5,5,5);
  gr->closeLine(10,10,10,10);
  gr->closeLine(110,110,110,110);
  gr->closeLine(457,92,457,92);
  gr->sendHoughToImage(rawImg);
  return 0;
  //*/
  
  
  
  //for (int i=0; i<200; i++) r->updateCoordsFile();
  for (int i=0; i<4000; i++){
    //std::cout << "---- " << i << " ----" << std::endl;
    l->triggerUpdate();
    
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    
    /*
    if (range[0]) g->closeSliceSide(drone[0], drone[1], weson[0], weson[1],angle);
    else g->openSliceSide(drone[0], drone[1], weson[0], weson[1],angle);
    
    if (range[1]) g->closeSliceFront(drone[0], drone[1], nwson[0], nwson[1],angle);
    else g->openSliceFront(drone[0], drone[1], nwson[0], nwson[1],angle);
    
    if (range[2]) g->closeSliceFront(drone[0], drone[1], neson[0], neson[1],angle);
    else g->openSliceFront(drone[0], drone[1], neson[0], neson[1],angle);
    
    if (range[3]) g->closeSliceSide(drone[0], drone[1], eason[0], eason[1],angle);
    else g->openSliceSide(drone[0], drone[1], eason[0], eason[1],angle);
    //*/
    
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    
    /*
    std::cout << sonarDists[0] << ",";
    std::cout << sonarDists[1] << ",";
    std::cout << sonarDists[2] << ",";
    std::cout << sonarDists[3] << std::endl; //*/
    
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    heading = (angle1 + angle2)/2.0;
    if (prevAngle == 0.0) prevAngle = heading;
    distA += sqrt((heading-prevAngle)*(heading-prevAngle));
    //angle = distA/1000.0 + 0.25;    //0.25
    angle = 0.17;
    prevAngle = heading;
    
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    
    /*
    OccupancyGrid *oo = a->generateMapNoBlur();
    char* name = new char[128];
    std::sprintf(name, "/home/owner/pics/fisheye/mapsA/p%07d.png", num);
    num += 1;
    oo->sendToImage(name, rawPos[0]/10, rawPos[1]/10);
    delete[] name;
    delete oo;
    std::cout << num << std::endl;
    //*/
    
    //use file for maps
    r->updateCoordsFile(); //*/
    
    /*//use virtual environment for maps
    v->changePosition(2,0);
    v->getCurrentCoordinates(buf1);
    buf1[0] += (int)XDrift;
    buf1[1] += (int)YDrift;
    
    XDrift += ((double)((std::rand() % 100) - 50)/10.0);
    YDrift += ((double)((std::rand() % 100) - 50)/10.0);
    
    r->updateCoordsVirtual(buf1);
    //*/
  }
  
  std::cout << "Generating occupancy grid..." << std::endl;
  OccupancyGrid* o1 = a->generateMapNoBlur();
  
  std::cout << "Generating hough map..." << std::endl;
  OccupancyGrid* hough = o1->generateHoughMap();
  //OccupancyGrid* o1 = a->generateMapReference();
  
  /*
  for (int i=0; i<5; i+=1) {
    char* name = new char[128];
    std::sprintf(name, "/home/owner/pics/pics/numpeaks/p%07d.png", i);
    hough = o1->generateHoughMap();
    hough->sendToImage(name,0,0);
    delete[] name;
    delete hough;
    std::cout << i << std::endl;
  }
  hough = o1->generateHoughMap();
  //*/
  
  
  
  
  
  
  
  std::cout << "Image" << std::endl;
  //o1->cleanFrontier();
  o1->sendHoughToImage(rawImg);
  //o1->sendToImage(occImg, 0,0);
  hough->sendToImage(sorImg, 0,0);
  //o1->sendHoughMaximaToImage(navImg);
  //o2->sendToImage(sorImg);
  //g->sendToImage(rawImg);
  //g->cleanFrontier();
  //g->sendToImage(navImg);
  std::cout << "Done!" << std::endl;
  
  /*
  g = a->generateMapReference();
  
  std::cout << "Image" << std::endl;
  g->sendToImage(imgFile2);
  std::cout << "Done!" << std::endl; //*/
  return 0; //*/
  
  return 0; //*/
  
}
