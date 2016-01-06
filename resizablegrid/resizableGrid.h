
#ifndef RG_RESIZABLE_GRID_
#define RG_RESIZABLE_GRID_

#include <iostream> //TODO: remove
#include <string>   //TODO: remove
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "nodes/node.h"
#include "./../pngwriter/png_writer.h"

class ResizableGrid {
  public:
    //occupancy variables
    static const char UNKNOWN = 0;
    static const char OPEN = 1;
    static const char CLOSED = -5;
    static const char FRONTIER = -128;
    static const char THRESHOLD = 0;
    
    //color in png output TODO: see if these are actually used anywhere
    static const int COLOR_UNKN = 0x888888ff;   //gray
    static const int COLOR_OPEN = 0xffffffff;   //white
    static const int COLOR_CLSD = 0x000080ff;   //blue
    static const int COLOR_FRON = 0xffff00ff;   //yellow
    static const int COLOR_EROR = 0xff0000ff;   //red
    
    //Gaussian kernels to use to blur image
    static const int NUM_KERNELS = 9;
    static const int KERNELS[][NUM_KERNELS + 1];
    static const int KERNEL_SUMS[];
    
    //other variables
    int x, y;   //position of root node
    Node *root; //root node
    
    //constructor
    ResizableGrid(int x, int y);
    
    //functions for adding to map
    void addClosed(int x1, int y1, int x2, int y2);
    void addFrontier(int x1, int y1, int x2, int y2);
    bool openNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode); //TODO: make this private
    void closeNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode);
    void frontierNodeLine(int relX1, int relY1, int relX2, int relY2, Node* curNode);
    
    //functions for adding sonar slices to map
    void openSlice(int relX1, int relY1, int relX2, int relY2, float angle, Node* curNode);
    void closeSlice(int relX1, int relY1, int relX2, int relY2, float angle, Node* curNode);
    
    //blur the map according to uncertainty
    void blurMapX(int uncertainty, Node* curNode);
    void blurMapY(int uncertainty, Node* curNode);
    
    //output map as image
    void sendToImage(char* filename);
    
  private:
    //move the root of the map with the quadcopter
    void moveRoot(char dir);
    
    //add a new node to the map
    void connectNewNode(Node* curNode, char side);
    
    //functions for moving around the map
    Node* getNode(int x, int y);
    Node* getNextNode(int relX, int relY, float dir, Node* curNode);
    void setImagePixel(PngWriter* w, int x, int y, char value);
    
};

#endif