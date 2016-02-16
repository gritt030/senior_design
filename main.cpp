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
  
  
  //Stuff for straightening maps
  for (int i=0; i<0; i++) r->updateCoordsFile();
  double origXcard = -1.0;
  double origYcard = -1.0;
  int counter = 100;
  SonarScan* scans = nullptr;
  SonarArchive* a2 = new SonarArchive();
  
  
  
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
    
    //stuff for hough alignment
    SonarScan* s = new SonarScan();
    s->w = sonarDists[0];
    s->nw = sonarDists[1];
    s->ne = sonarDists[2];
    s->e = sonarDists[3];
    s->x = rawPos[0];
    s->y = rawPos[1];
    s->xErr = distX/100.0;
    s->yErr = distY/100.0;
    s->heading = heading;
    s->headErr = angle;
    s->previous = scans;
    scans = s;
    counter--;
    
    if (counter == 0) {
      OccupancyGrid* ocg = a->generateMapNoBlur();
      OccupancyGrid* hgh = ocg->generateHoughMap();
      if (hgh->Y_Cardinal > 1.5) hgh->Y_Cardinal -= 3.141592654;
      delete a;
      a = new SonarArchive();
            
      if (origXcard < 0) {
        std::cout << "set: \n";
        origXcard = hgh->X_Cardinal;
        origYcard = hgh->Y_Cardinal;
        std::cout << "    " << origXcard << std::endl;
        std::cout << "    " << origYcard << std::endl;
        ocg->sendToImage(navImg, 0,0);
        hgh->sendToImage(rawImg, 0,0);
      } else {
        double hdErr = (hgh->X_Cardinal) - origXcard;
        hdErr += (hgh->Y_Cardinal) - origYcard;
        hdErr /= 2.0;
        double co = cos(hdErr);
        double si = sin(hdErr);
        double x2, y2;
        std::cout << "  " << hgh->X_Cardinal << ", " << hgh->Y_Cardinal << ", " << hdErr << std::endl;
        s = scans;
        while (s != nullptr){
          s->heading += hdErr;
          x2 = (s->x)*co - (s->y)*si;
          y2 = (s->x)*si + (s->y)*co;
          s->x = x2;
          s->y = y2;
          s = s->previous;
        }
      }
        
        s = scans;
        SonarScan* t;
        while (scans != nullptr){
          t = scans->previous;
          if (t != nullptr) {
            while (t->previous != nullptr) {
              s = s->previous;
              t = t->previous;
            }
          } else {
            t = s;
            scans = nullptr;
          }
          
          int* ssd = new int[4];
          ssd[0] = t->w;
          ssd[1] = t->nw;
          ssd[2] = t->ne;
          ssd[3] = t->e;
          a2->addSonarScan(ssd, t->x, t->y, t->xErr, t->yErr, t->heading, t->headErr);
          delete[] ssd;
          s->previous = nullptr;
          s = scans;
          delete t;
        }
      
      counter = 100;
      delete ocg;
      delete hgh;
    }
    
    
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
  OccupancyGrid* o1 = a2->generateMapNoBlur();
  
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
  //o1->sendHoughToImage(rawImg);
  o1->sendToImage(occImg, 0,0);
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
