#include "config.h"

Config::Config( const std::string fileName)
{
  std::ifstream configFile( fileName);
  std::string line;
  if ( configFile.is_open()) {
    while ( std::getline( configFile, line)) {
      size_t i = line.find( '%');
      size_t j = line.find_first_of( '=');
      if ( i != std::string::npos) {
        // Skip comments
        continue;
      }
      else if ( j == std::string::npos) {
        // Skip lines without variable assignment
        continue;
      }
      std::string variableName = line.substr( 0, j);
      std::string variableValue = line.substr( j+1);
      assignVariable( variableName, variableValue);
    }
  }
  else {
    std::cerr << "\033[1;31m"
              << "Could not locate configuration file \"" << fileName << "\".\n"
              << "\033[0m";
  }
  configFile.close();
}

void Config::assignVariable( std::string &var, std::string &val)
{
  if ( var.compare("udp_port") == 0) {
    udpPort = std::stod( val);
  }
  else if ( var.compare("serial_port") == 0) {
    serialPort = val;
  }
  else if ( var.compare("latitude") == 0) {
    latitude = std::stod( val) * M_PI/180;
  }
  else if ( var.compare("longitude") == 0) {
    longitude = std::stod( val) * M_PI/180;
  }
  else if ( var.compare("altitude_AMSL") == 0) {
    altitudeAmsl = std::stod( val);
  }
  else if ( var.compare("magnetic_declination") == 0) {
    magneticDeclination = std::stod( val) * M_PI/180;
  }
}
