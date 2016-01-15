#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "occupancygrid/grid/grid.h"
#include "occupancygrid/occupancygrid.h"
#include "localization/coordinatereader.h"
#include "localization/localizer.h"
#include "navigation/virtualenvironment.h"
#include "sonar/sonarscan.h"
#include "sonar/sonararchive.h"

#include "pngwriter/png_writer.h"


int main(int argc, char **argv) {
  
  char* imgFile = "/home/owner/pics/large/2Estimate.png";
  char* imgFile2 = "/home/owner/pics/large/2EstGlobalSort.png";
  char* coordFile = "/home/owner/workspace/Datasets/output_ds3/coordsAccurate.txt";
  
  CoordinateReader* r = new CoordinateReader(coordFile);
  r->updateCoordsFile();
  
  Localizer* l = new Localizer(r);
  OccupancyGrid* g = new OccupancyGrid();
  SonarArchive* a = new SonarArchive();
  
  int* drone = new int[3];
  int* weson = new int[3];
  int* eason = new int[3];
  int* nwson = new int[3];
  int* neson = new int[3];
  bool* range = new bool[4];
  float angle = 0.13;
  double distX = 0.0;
  double distY = 0.0;
  double divX=1.0, divY=1.0;
  int* prevLoc = new int[3];
  int* sonarDists = new int[4];
  double* rawPos = new double[5];
  
  for (int i=0; i<4000; i++){
    //std::cout << "---- " << i << " ----" << std::endl;
    //std::cout << "    Position" << std::endl;
    l->triggerUpdate();
    l->getPosition(drone);
    
    distX += sqrt((drone[0]-prevLoc[0])*(drone[0]-prevLoc[0]));
    if (distX/divX >= 100.0){
      std::cout << "Blur X: " << distX << std::endl;
      divX += 1.0;
      g->blurMapX(1);
    }
    
    distY += sqrt((drone[1]-prevLoc[1])*(drone[1]-prevLoc[1]));
    if (distY/divY >= 100.0){
      std::cout << "Blur Y: " << distY << std::endl;
      divY += 1.0;
      g->blurMapY(1);
    }
    
    prevLoc[0] = drone[0];
    prevLoc[1] = drone[1];
    
    //std::cout << "    Sonar" << std::endl;
    l->getWSonarPosition(weson);
    l->getESonarPosition(eason);
    l->getNWSonarPosition(nwson);
    l->getNESonarPosition(neson);
    l->getSonarInRange(range);
    
    //std::cout << range[0] << range[1] << range[2] << range[3] << std::endl;
    
    //std::cout << "    Pos: " << drone[0] << ", " << drone[1] << std::endl;
    //std::cout << "    Son: " << weson[0] << ", " << weson[1] << std::endl;
    //std::cout << "    Line" << std::endl;
    
    if (range[0]) g->closeSlice(drone[0], drone[1], weson[0], weson[1],angle);
    else g->openSlice(drone[0], drone[1], weson[0], weson[1],angle);
    
    if (range[1]) g->closeSlice(drone[0], drone[1], nwson[0], nwson[1],angle);
    else g->openSlice(drone[0], drone[1], nwson[0], nwson[1],angle);
    
    if (range[2]) g->closeSlice(drone[0], drone[1], neson[0], neson[1],angle);
    else g->openSlice(drone[0], drone[1], neson[0], neson[1],angle);
    
    if (range[3]) g->closeSlice(drone[0], drone[1], eason[0], eason[1],angle);
    else g->openSlice(drone[0], drone[1], eason[0], eason[1],angle);
    
    
    l->getRawSonarDists(sonarDists);
    l->getRawPosition(rawPos);
    
    if (distX > 6000.0) {
      distX -= 600.0;
      divX = 1.0;
    }
    
    if (distY > 6000.0) {
      distY -= 600.0;
      divY = 1.0;
    }
    
    /*
    std::cout << drone[0] << ", " << drone[1] << std::endl; //*/
    
    double angle1 = atan2(nwson[1]-drone[1], nwson[0]-drone[0]);
    double angle2 = atan2(neson[1]-drone[1], neson[0]-drone[0]);
    a->addSonarScan(sonarDists, rawPos[0], rawPos[1], distX/10.0, distY/10.0, (angle1+angle2)/2.0, angle);
    
    //std::cout << "    Next" << std::endl;
    r->updateCoordsFile();
  }
  
  std::cout << "Image" << std::endl;
  g->sendToImage(imgFile);
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
  
  std::cout << "Creating image..." << std::endl;
  
  g->sendToImage(imgFile);
  
  std::cout << "Done!" << std::endl;
  return 0; //*/
  
}
