
#ifndef SONARSCAN_H_
#define SONARSCAN_H_

class SonarScan
{
public:
  //sonar distances
  unsigned short w, nw, ne, e;
  
  //position readings were made at
  double x, y;
  
  //direction facing during measurements
  double heading;
  
  //errors in position and heading
  double xErr, yErr, headErr;
  
  //previous scan in chain
  SonarScan* previous;
};

#endif // SONARSCAN_H
