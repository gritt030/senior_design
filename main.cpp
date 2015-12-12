#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "resizablegrid/nodes/node.h"
#include "resizablegrid/resizableGrid.h"
#include "localization/coordinatereader.h"
#include "localization/localizer.h"

#include "pngwriter/png_writer.h"


int main(int argc, char **argv) {
  
  char* imgFile = "/home/owner/pics/cornerfix.png";
  char* coordFile = "/home/owner/workspace/Datasets/coords/coords_corner.txt";
  
  CoordinateReader* r = new CoordinateReader(coordFile);
  r->goToNextCoords();
  
  Localizer* l = new Localizer(r);
  ResizableGrid* g = new ResizableGrid(0, 0);
  
  int* drone = new int[2];
  int* weson = new int[2];
  int* eason = new int[2];
  int* nwson = new int[2];
  int* neson = new int[2];
  
  for (int i=0; i<1000; i++){
    std::cout << "---- " << i << " ----" << std::endl;
    //std::cout << "    Position" << std::endl;
    l->getPosition(drone);
    
    //std::cout << "    Sonar" << std::endl;
    l->getWSonarPosition(weson);
    weson[0] += drone[0];
    weson[1] += drone[1];
    l->getESonarPosition(eason);
    eason[0] += drone[0];
    eason[1] += drone[1];
    l->getNWSonarPosition(nwson);
    nwson[0] += drone[0];
    nwson[1] += drone[1];
    l->getNESonarPosition(neson);
    neson[0] += drone[0];
    neson[1] += drone[1];
    
    //std::cout << "    Pos: " << drone[0] << ", " << drone[1] << std::endl;
    //std::cout << "    Son: " << wSonarCoords[0] << ", " << wSonarCoords[1] << std::endl;
    //std::cout << "    Line" << std::endl;
    g->closeNodeLine(drone[0], drone[1], weson[0], weson[1], g->root);
    g->closeNodeLine(drone[0], drone[1], eason[0], eason[1], g->root);
    g->closeNodeLine(drone[0], drone[1], nwson[0], nwson[1], g->root);
    g->closeNodeLine(drone[0], drone[1], neson[0], neson[1], g->root);
    
    //std::cout << "    Next" << std::endl;
    r->goToNextCoords();
  }
  
  std::cout << "Image" << std::endl;
  g->sendToImage(imgFile);
  std::cout << "Done!" << std::endl;
  return 0;
  
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
   * FIX OVERFLOW JUNK
   * ALSO SET vs. INCREMENT
   * DO ALL THE THINGS
   */
  
  /*
  Node* n = new Node();
  ResizableGrid* g = new ResizableGrid(0, 0);
  
  std::cout << "Resizable Grid:" << std::endl;
  std::cout << "    Node Size: " << sizeof(Node) << std::endl;
  std::cout << "    ResG Size: " << sizeof(ResizableGrid) << std::endl;
  std::cout << std::endl;
  
  std::cout << "Drawing..." << std::endl;
  
  g->closeNodeLine(0,0,0,0, g->root);
  
  for(int i=0; i<70; i++){
  g->closeNodeLine(0,20,10,0, g->root);
  g->closeNodeLine(0,20,-10,0, g->root);
  g->closeNodeLine(10,0,0,-20, g->root);
  g->closeNodeLine(-10,0,0,-20, g->root);
  }
  
  for(int i=0; i<70; i++){
  g->closeNodeLine(0,10,20,0, g->root);
  g->closeNodeLine(0,10,-20,0, g->root);
  g->closeNodeLine(20,0,0,-10, g->root);
  g->closeNodeLine(-20,0,0,-10, g->root);
  }
  
  g->closeNodeLine(-20,20,20,20, g->root);
  g->closeNodeLine(20,20,20,-20, g->root);
  g->closeNodeLine(-20,-20,-20,20, g->root);
  g->closeNodeLine(20,-20,-20,-20, g->root);
  
  g->closeNodeLine(0,0,30,30, g->root);
  g->closeNodeLine(0,0,-30,30, g->root);
  g->closeNodeLine(0,0,-30,-30, g->root);
  g->closeNodeLine(0,0,30,-30, g->root);
  
  for (int i=0; i<70; i++){
    g->closeNodeLine(0,0,0,0, g->root);
  }
  
  std::cout << "Creating image..." << std::endl;
  
  g->sendToImage(imgFile);
  
  std::cout << "Done!" << std::endl;
  return 0; //*/
  
}
