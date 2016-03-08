#ifndef HOUGHLINE_H
#define HOUGHLINE_H

class HoughLine
{
public:
  double radius, theta;
  int startX, startY, endX, endY;
  HoughLine* next;
};

#endif // HOUGHLINE_H
