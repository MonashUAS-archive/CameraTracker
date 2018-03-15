#pragma once

#include <string>
#include <fstream>
#include <iostream>

class Config
{
public:
  explicit Config( const std::string fileName);

  int udpPort = 14551;
  std::string serialPort = "ACM0";
  double latitude = 0.0;
  double longitude = 0.0;
  double altitudeAmsl = 0.0;
  double magneticDeclination = 11.79;

private:
  void assignVariable( std::string &var, std::string &val);

};
