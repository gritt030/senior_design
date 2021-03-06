
#ifndef OG_OCCUPANCY_GRID_
#define OG_OCCUPANCY_GRID_

#include <iostream> //TODO: remove
#include <string>   //TODO: remove
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "grid/grid.h"
#include "grid/houghgrid.h"
#include "grid/houghleastsquares.h"
#include "grid/houghline.h"
#include "./../pngwriter/png_writer.h"

class OccupancyGrid {
  public:
    //occupancy variables
    static const char UNKNOWN = 0;
    static const char OPEN = 1;
    static const char CLOSED = -10;
    static const char THRESHOLD = 0;
    static const char GHOSTS = 40;
    
    static const char FRONT_THRESH = 0;
    static const char FRONTIER = 1;
    static const char UNFRONTIER = -1;
    
    //used to correct coordinates when setting values in grid
    static const int BOUNDARY = Grid::GRID_SIZE / 2;
    
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
    
    //TODO: Make private
    //other variables
    Grid *grid; //occupancy grid
    Grid *frontiers;    //frontier grid
    
    //constructor/destructor
    OccupancyGrid();
    ~OccupancyGrid();
    
    //functions for adding to map
    bool openLine(int relX1, int relY1, int relX2, int relY2); //TODO: make this private
    void closeLine(int relX1, int relY1, int relX2, int relY2);
    bool openLineFrontier(int relX1, int relY1, int relX2, int relY2); //TODO: make this private
    void closeLineFrontier(int relX1, int relY1, int relX2, int relY2);
    
    //functions for adding sonar slices to map
    void openSlice(int relX1, int relY1, int relX2, int relY2, float angle);
    void closeSlice(int relX1, int relY1, int relX2, int relY2, float angle);
    void openSliceFrontier(int relX1, int relY1, int relX2, int relY2, float angle);
    void closeSliceFrontier(int relX1, int relY1, int relX2, int relY2, float angle);
    
    //blur the map according to uncertainty
    void blurMapX(int uncertainty);
    void blurMapY(int uncertainty);
    
    //combine two occupancy maps
    void mergeMaps(OccupancyGrid* newGrid);
    void overlayMaps(OccupancyGrid* newGrid);
    
    //draw lines in map based on lines in ref map
    bool openLineAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2);
    void closeLineAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2);
    void openSliceAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2, float angle);
    void closeSliceAccording(OccupancyGrid* ref, int relX1, int relY1, int relX2, int relY2, float angle);

    //copy just the walls of the occupancy map into newGrid
    void getWallMap(OccupancyGrid* newGrid);
    //copy just the open areas of the occupancy map into newGrid
    void getOpenMap(OccupancyGrid* newGrid);
    
    //output map as image
    void sendToImage(char* filename, int x, int y);
    void sendToImageFrontier(char* filename);
    void sendToImageSmall(char* filename);
    
    void cleanFrontier();
    
    
    
    
    
    
    
    
    //LSD line detection stuff
    OccupancyGrid* fitLinesLSD();
    
    
    
    
    
    
    
    //Hough transform stuff
//     typedef struct HoughLine{
//       double radius, theta;
//       int startX, startY, endX, endY;
//       struct HoughLine* next;
//     } HoughLine;
    
    HoughLine* lineList = nullptr;
    HoughLine* getHoughLines(double radius, double theta);
    void makeHoughLine(double r, double t, int sX, int sY, int eX, int eY, HoughLine* hl);
    int combineHoughLines(HoughLine* lines);
    int collapseLineList();
    int mergeLineList(HoughLine* lines);
    bool mergeHoughLines(HoughLine* line1, HoughLine* line2);
    void traceHoughWalls(OccupancyGrid* newGrid, HoughLeastSquares* houghLS);
    void drawHoughLine(HoughLine* line, OccupancyGrid* newGrid);
    
    OccupancyGrid* generateHoughMap();
    void traceHoughWalls(OccupancyGrid* newGrid, HoughGrid* hough, HoughLeastSquares* houghLS);
    void traceHoughLine(double radius, double theta, double lsRadius, double lsTheta, OccupancyGrid* newGrid);
    void traceHoughLine(double radius, double theta, OccupancyGrid* newGrid);
    void traceCardinalDirections(double x_card, double y_card, OccupancyGrid* newGrid);
    void sendHoughToImage(char* filename);
    void sendHoughMaximaToImage(char* filename);
    void setImagePixelHough(PngWriter* w, int x, int y, unsigned short value);
    HoughGrid* performHoughTransform();
    double getRotationValue();
    static const int BRIDGE = 36;//12;   //number of open cells to jump when tracing lines (12)
    static const int MIN_BRIDGE = 81;//5;    //number of closed cells in a row required to make a wall (5)
    
    //theta used for cardinal directions
    double X_Cardinal=-1.0, Y_Cardinal=-1.0;
    
  private:
    //set a pixel in the image
    void setImagePixel(PngWriter* w, int x, int y, char value);
    void setImagePixelFrontier(PngWriter* w, int x, int y, char value, char front);
    
    void removeGhosts(int relX1, int relY1, int relX2, int relY2);
    
};

#endif