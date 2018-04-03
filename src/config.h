#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <math.h>

class Config
{
public:
  explicit Config( const std::string fileName);

  int udpPort = 14551;
  std::string serialPort = "ACM0";
  double latitude = 0.0;                           // rad
  double longitude = 0.0;                          // rad
  double altitudeAmsl = 0.0;                       // m
  double magneticDeclination = 11.79 * M_PI/180;   // rad

private:
  void assignVariable( std::string &var, std::string &val);

};
