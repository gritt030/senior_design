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
  char* name = new char[256];
  
  //std::srand(std::time(nullptr));
  int num = 0;
  
  
  /*
  //Stuff for straightening maps
  //for (int i=0; i<0; i++) r->updateCoordsFile();
  double origXcard = -1.0;
  double origYcard = -1.0;
  int counter = 10;
  SonarScan* scans = nullptr;
  SonarArchive* a2 = new SonarArchive();
  OccupancyGrid* eternalHough;
  
  std::cout << "Initial loop...\n";
  //straightening initial loop
  for (int i=0; i<1000; i++){
    l->triggerUpdate();
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    heading = (angle1 + angle2)/2.0;
    if (prevAngle == 0.0) prevAngle = heading;
    distA += sqrt((heading-prevAngle)*(heading-prevAngle));
    angle = 0.17;
    prevAngle = heading;
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    //a2->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    
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
    
    r->updateCoordsFile();
  }
  
  std::cout << "Middle loop...\n";
  //stuff used for straightening
  OccupancyGrid* ocg = a->generateMapNoBlur();
  OccupancyGrid* hgh = ocg->generateHoughMap();
  eternalHough = hgh;
  std::cout << "set: \n";
  origXcard = hgh->X_Cardinal;
  origYcard = hgh->Y_Cardinal;
  std::cout << "    " << origXcard << std::endl;
  std::cout << "    " << origYcard << std::endl;
  delete ocg;
  //delete hgh;
  
  
  SonarScan* s;
  ocg = a->generateMapNoBlur();
  hgh = ocg->generateHoughMap();
  double hdErr = (hgh->X_Cardinal) - 1.570796327;//origXcard;
  double co = cos(hdErr);
  double si = sin(hdErr);
  double x2, y2;
      
  SonarScan* t = scans;
  s = scans;
  
  for (int i=0; i<1000; i++){
    t->heading += hdErr;
    x2 = (t->x)*co - (t->y)*si;
    y2 = (t->x)*si + (t->y)*co;
    t->x = x2;
    t->y = y2;
    
    int* ssd = new int[4];
    ssd[0] = t->w;
    ssd[1] = t->nw;
    ssd[2] = t->ne;
    ssd[3] = t->e;
    a2->addSonarScan(ssd, t->x, t->y, t->xErr, t->yErr, t->heading, t->headErr);
    t = t->previous;
  }
  scans = nullptr;
  delete ocg;
  delete hgh;
  
  
  
  
  //3577, 1633
  std::cout << "Secondary loop...\n";
  //straightening secondary loop
  for (int i=0; i<300; i++){
    l->triggerUpdate();
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    heading = (angle1 + angle2)/2.0;
    if (prevAngle == 0.0) prevAngle = heading;
    distA += sqrt((heading-prevAngle)*(heading-prevAngle));
    angle = 0.17;
    prevAngle = heading;
    a->removeSonarScan();
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
      OccupancyGrid* ocg = a2->generateMapNoBlur();
      OccupancyGrid* hgh = ocg->generateHoughMap();
      std::sprintf(name, "/home/owner/pics/pics/sequence/p%07d.png", i);
      hgh->sendToImage(name, 0,0);
      std::sprintf(name, "/home/owner/pics/pics/sequence/r%07d.png", i);
      ocg->sendToImage(name, 0,0);
      origXcard = hgh->X_Cardinal;
      origYcard = hgh->Y_Cardinal;
      delete ocg;
      delete hgh;
      ocg = a->generateMapNoBlur();
      hgh = ocg->generateHoughMap();
      //eternalHough->mergeMaps(hgh);
      double hdErr = (hgh->X_Cardinal) - 1.570796327;//origXcard;
      double co = cos(hdErr);
      double si = sin(hdErr);
      double x2, y2;
      std::cout << i << ":  " << hgh->X_Cardinal << ", " << hgh->Y_Cardinal << ", " << hdErr << std::endl;
      
      SonarScan* t = scans;
      s = scans;
      
      while (t != nullptr) {        
        while (t->previous != nullptr){
          s = t;
          t = t->previous;
        }
        t->heading += hdErr;
        x2 = (t->x)*co - (t->y)*si;
        y2 = (t->x)*si + (t->y)*co;
        t->x = x2;
        t->y = y2;
        
        int* ssd = new int[4];
        ssd[0] = t->w;
        ssd[1] = t->nw;
        ssd[2] = t->ne;
        ssd[3] = t->e;
        a2->addSonarScan(ssd, t->x, t->y, t->xErr, t->yErr, t->heading, t->headErr);
        
        if (s->previous == nullptr){
          scans = nullptr;
        } else {
          s->previous = nullptr;
        }
        
        delete t;
        delete[] ssd;
        t = scans;
        s = scans;
      }
      
      delete ocg;
      delete hgh;
      counter = 10;
    }
    
    r->updateCoordsFile();
  }
  
  std::cout << "Generating occupancy grid..." << std::endl;
  OccupancyGrid* og = a2->generateMapNoBlur();
  
  std::cout << "Generating hough map..." << std::endl;
  OccupancyGrid* hg = og->generateHoughMap();
  
  std::cout << "Image" << std::endl;
  og->sendToImage(occImg, 0,0);
  hg->sendToImage(navImg, 0,0);
  //eternalHough->sendToImage(navImg, 0,0);
  std::cout << "Done!" << std::endl;
  return 0; //*/
  
  
  
  
  
  
  
  
  
  /*
  for (int i=0; i<500; i++) r->updateCoordsFile();
  //Stuff for straightening maps VERSION 2
  //for (int i=0; i<0; i++) r->updateCoordsFile();
  double origXcard = -1.0;
  double origYcard = -1.0;
  int counter = 10;
  SonarScan* scans = nullptr;
  SonarArchive* a2 = new SonarArchive();
  
  int initialSize = 300;
  
  std::cout << "Initial loop...\n";
  //straightening initial loop
  for (int i=0; i<initialSize; i++){
    l->triggerUpdate();
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    heading = (angle1 + angle2)/2.0;
    if (prevAngle == 0.0) prevAngle = heading;
    distA += sqrt((heading-prevAngle)*(heading-prevAngle));
    angle = 0.17;
    prevAngle = heading;
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    //a2->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    
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
    
    r->updateCoordsFile();
  }
  
  std::cout << "Middle loop...\n";
  //stuff used for straightening
  OccupancyGrid* ocg = new OccupancyGrid();
  a->addPath(ocg);
  OccupancyGrid* hgh = ocg->generateHoughMap();
  std::cout << "set: \n";
  origXcard = hgh->X_Cardinal;
  origYcard = hgh->Y_Cardinal;
  std::cout << "    " << origXcard << std::endl;
  std::cout << "    " << origYcard << std::endl;
  delete ocg;
  delete hgh;
  
  
  SonarScan* s;
  ocg = new OccupancyGrid();
  a->addPath(ocg);
  hgh = ocg->generateHoughMap();
  double hdErr = (hgh->X_Cardinal) - 1.570796327;//origXcard;
  double co = cos(hdErr);
  double si = sin(hdErr);
  double x2, y2;
      
  SonarScan* t = scans;
  s = scans;
  
  for (int i=0; i<initialSize; i++){
    t->heading += hdErr;
    x2 = (t->x)*co - (t->y)*si;
    y2 = (t->x)*si + (t->y)*co;
    t->x = x2;
    t->y = y2;
    
    int* ssd = new int[4];
    ssd[0] = t->w;
    ssd[1] = t->nw;
    ssd[2] = t->ne;
    ssd[3] = t->e;
    a2->addSonarScan(ssd, t->x, t->y, t->xErr, t->yErr, t->heading, t->headErr);
    t = t->previous;
  }
  scans = nullptr;
  delete ocg;
  delete hgh;
  
  
  
  
  //3577, 1633
  std::cout << "Secondary loop...\n";
  //straightening secondary loop
  for (int i=0; i<700; i++){
    l->triggerUpdate();
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    heading = (angle1 + angle2)/2.0;
    if (prevAngle == 0.0) prevAngle = heading;
    distA += sqrt((heading-prevAngle)*(heading-prevAngle));
    angle = 0.17;
    prevAngle = heading;
    a->removeSonarScan();
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
      ocg = new OccupancyGrid();
      a->addPath(ocg);
      hgh = ocg->generateHoughMap();
      std::sprintf(name, "/home/owner/pics/pics/sequence/r%07d.png", i);
      hgh->sendToImage(name, 0,0);
      std::sprintf(name, "/home/owner/pics/pics/sequence/p%07d.png", i);
      ocg->sendToImage(name, 0,0);
      double hdErr = (hgh->X_Cardinal) - 1.570796327;//origXcard;
      double co = cos(hdErr);
      double si = sin(hdErr);
      double x2, y2;
      std::cout << i << ":  " << hgh->X_Cardinal << ", " << hgh->Y_Cardinal << ", " << hdErr << std::endl;
      
      SonarScan* t = scans;
      s = scans;
      
      while (t != nullptr) {        
        while (t->previous != nullptr){
          s = t;
          t = t->previous;
        }
        t->heading += hdErr;
        x2 = (t->x)*co - (t->y)*si;
        y2 = (t->x)*si + (t->y)*co;
        t->x = x2;
        t->y = y2;
        
        int* ssd = new int[4];
        ssd[0] = t->w;
        ssd[1] = t->nw;
        ssd[2] = t->ne;
        ssd[3] = t->e;
        a2->addSonarScan(ssd, t->x, t->y, t->xErr, t->yErr, t->heading, t->headErr);
        
        if (s->previous == nullptr){
          scans = nullptr;
        } else {
          s->previous = nullptr;
        }
        
        delete t;
        delete[] ssd;
        t = scans;
        s = scans;
      }
      
      delete ocg;
      delete hgh;
      counter = 10;
    }
    
    r->updateCoordsFile();
  }
  
  std::cout << "Generating occupancy grid..." << std::endl;
  OccupancyGrid* og = a2->generateMapNoBlur();
  
  std::cout << "Generating hough map..." << std::endl;
  OccupancyGrid* hg = og->generateHoughMap();
  
  std::cout << "Image" << std::endl;
  og->sendToImage(occImg, 0,0);
  hg->sendToImage(navImg, 0,0);
  //eternalHough->sendToImage(navImg, 0,0);
  std::cout << "Done!" << std::endl;
  return 0; //*/
  
  
  
  
  
  
  
  
  
  
  
  /////Normal main loop /////
  //for (int i=0; i<950; i++) r->updateCoordsFile();
  for (int i=0; i<3000; i++){
    //std::cout << "---- " << i << " ----" << std::endl;
    l->triggerUpdate();
    
    l->getPosition(drone);
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    distX += sqrt((rawPos[0]-prevLoc[0])*(rawPos[0]-prevLoc[0]));
    distY += sqrt((rawPos[1]-prevLoc[1])*(rawPos[1]-prevLoc[1]));
    prevLoc[0] = rawPos[0];
    prevLoc[1] = rawPos[1];
    
//     sonarDists[0] = 0;
//     sonarDists[1] = 0;
//     sonarDists[2] = 0;
//     sonarDists[3] = 0;
    
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
    angle = 0.27;
    prevAngle = heading;
    
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    
//     if (i%1 == 0){
//       sprintf(name, "/home/owner/pics/pics/sequence/p%07d.png", i);
//       OccupancyGrid* grd = a->generateMapNoBlur();
//       grd->sendToImage(name, rawPos[0]/10, rawPos[1]/10);
//       delete grd;
//     }
    
    
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
  
//   for (int i=0; i<100; i++){
//     OccupancyGrid* o1 = a->generateMapNoBlur();
//     std::sprintf(name, "/home/owner/pics/pics/sequence/p%07d.png", i);
//     a->addPath(o1);
//     o1->sendToImage(name, 0,0);
//     delete o1;
//     a->shiftScans();
//   }
  
  std::cout << "Generating occupancy grid..." << std::endl;
  OccupancyGrid* o1 = a->generateMapNoBlur();
  
  std::cout << "Generating hough map..." << std::endl;
  OccupancyGrid* hough = o1->generateHoughMap();
  
  std::cout << hough->Y_Cardinal << std::endl;
  a->rotateMap(0.18326);
  //a->rotateMap(hough->Y_Cardinal);
  delete o1;
  delete hough;
  o1 = a->generateMapNoBlur();
  hough = o1->generateHoughMap();
  
  a->addPath(o1);
  a->addPath(hough);
  
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
  hough->sendToImage(navImg, 0,0);
  //a->generateMapNoBlur()->sendHoughMaximaToImage(sorImg);
  
  //a->generateMapNoBlur()->generateHoughMap()->generateHoughMap()->sendToImage(sorImg, 0,0);
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
