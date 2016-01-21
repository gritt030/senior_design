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

#include "pngwriter/png_writer.h"


int main(int argc, char **argv) {
  if (argc != 2) return 0;
  
  char* occImg = "/home/owner/pics/presentation/occupancy.png";
  char* sorImg = "/home/owner/pics/presentation/refined.png";
  char* rawImg = "/home/owner/pics/presentation/rawnav.png";
  char* navImg = "/home/owner/pics/presentation/navigate.png";
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
  SonarArchive* a = new SonarArchive();
  
  int* drone = new int[3];
  int* weson = new int[3];
  int* eason = new int[3];
  int* nwson = new int[3];
  int* neson = new int[3];
  bool* range = new bool[4];
  float angle = 0.25;
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
  
//   for (int i=0; i<1500; i++) r->updateCoordsFile();
  
  for (int i=0; i<4000; i++){
    //std::cout << "---- " << i << " ----" << std::endl;
    l->triggerUpdate();
    l->getPosition(drone);
    
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    
    if (range[0]) g->closeSliceSide(drone[0], drone[1], weson[0], weson[1],angle);
    else g->openSliceSide(drone[0], drone[1], weson[0], weson[1],angle);
    
    if (range[1]) g->closeSliceFront(drone[0], drone[1], nwson[0], nwson[1],angle);
    else g->openSliceFront(drone[0], drone[1], nwson[0], nwson[1],angle);
    
    if (range[2]) g->closeSliceFront(drone[0], drone[1], neson[0], neson[1],angle);
    else g->openSliceFront(drone[0], drone[1], neson[0], neson[1],angle);
    
    if (range[3]) g->closeSliceSide(drone[0], drone[1], eason[0], eason[1],angle);
    else g->openSliceSide(drone[0], drone[1], eason[0], eason[1],angle);
    
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
    //angle = distA/1000.0 + 0.25;    //0.25
    angle = 0.25;
    prevAngle = heading;
    
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/100.0, distY/100.0, heading, angle);
    
//     if (i%1 == 0) {
//     OccupancyGrid* o1 = a->generateMapNoBlur();
//     char* name = new char[128];
//     sprintf(name, "/home/owner/pics/presentation/%d.png", i);
//     o1->sendToImage(name);
//     delete name;
//     delete o1;
//     o1 = a->generateMapNoBlur();
//     delete o1;
//     }
    
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
  //OccupancyGrid* o1 = a->generateMapReference();

  std::cout << "Image" << std::endl;
  o1->cleanFrontier();
  o1->sendToImageSmall(occImg);
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
  
  /*
  PngWriter* w = new PngWriter();
  w->create_image(file, 500, 500);
  w->set_pixel(100,100,0xffffffff);
  w->set_pixel(10,50,0xff000000);
  w->set_pixel(300,60,0xff00ff00);
  w->output_image();
  return 0;
  //*/
  
  
  /*
  Node* n = new Node();
  ResizableGrid* g = new ResizableGrid(0, 0);
  
  std::cout << "Resizable Grid:" << std::endl;
  std::cout << "    Node Size: " << sizeof(Node) << std::endl;
  std::cout << "    ResG Size: " << sizeof(ResizableGrid) << std::endl;
  std::cout << std::endl;
  
  std::cout << "Drawing..." << std::endl;
  
  g->openSlice(-64,-73,-43,-93,0.0243265, g->root);
  g->openSlice(-73,-64,-93,-43,0.0243265, g->root);
  
  srand(time(nullptr));
  for (int i=0; i<500; i++){
    int x1 = rand()%(Node::GRID_SIZE*2) - Node::GRID_SIZE;
    int x2 = rand()%(Node::GRID_SIZE*2) - Node::GRID_SIZE;
    int x3 = rand()%(Node::GRID_SIZE*2) - Node::GRID_SIZE;
    int x4 = rand()%(Node::GRID_SIZE*2) - Node::GRID_SIZE;
    float angle = (float)rand()/(float)rand();
    std::cout << i << std::endl;
    //std::cout << x1 << "," <<x2<<","<<x3<<","<<x4<<","<<angle<<std::endl;
    g->openSlice(x1, x2, x3, x4, angle, g->root);
  }
  
  g->openSlice(0,0,20,10, 0.1745329252, g->root);
  g->openSlice(0,0,10,20, 0.1745329252, g->root);
  
  for(int i=0; i<70; i++){
  g->openNodeLine(0,20,10,0, g->root);
  g->openNodeLine(0,20,-10,0, g->root);
  g->openNodeLine(10,0,0,-20, g->root);
  g->openNodeLine(-10,0,0,-20, g->root);
  }
  
  for(int i=0; i<70; i++){
  g->openNodeLine(0,10,20,0, g->root);
  g->openNodeLine(0,10,-20,0, g->root);
  g->openNodeLine(20,0,0,-10, g->root);
  g->openNodeLine(-20,0,0,-10, g->root);
  }
  
  g->openNodeLine(-20,20,20,20, g->root);
  g->openNodeLine(20,20,20,-20, g->root);
  g->openNodeLine(-20,-20,-20,20, g->root);
  g->openNodeLine(20,-20,-20,-20, g->root);
  
  g->openNodeLine(0,0,30,30, g->root);
  g->openNodeLine(0,0,-30,30, g->root);
  g->openNodeLine(0,0,-30,-30, g->root);
  g->openNodeLine(0,0,30,-30, g->root);
  
  for (int i=0; i<70; i++){
    g->closeNodeLine(0,0,0,0, g->root);
  } //*/
  
  return 0; //*/
  
}
