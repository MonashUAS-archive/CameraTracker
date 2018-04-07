// MAVLink includes
#include "include/c_library_v2/common/mavlink.h"

// Arduino includes
#include <SD.h>
#include <SPI.h>

// Local includes
#include "src/matrix33.h"
#include "src/vector31.h"

// Geodetic constants
const float a = 6378137.0;
const float f = 1/298.257223563;
const float b = a*(1-f);

struct VehiclePosition
{
  float lat = 0;  // geodetic latitude [rad]
  float lon = 0;  // longitude [rad]
  float alt = 0;  // altitude above mean sea level [m]
  float vx = 0;   // body velocity X [m/s]
  float vy = 0;   // body velocity Y [m/s]
  float vz = 0;   // body velocity Z [m/s]
  Vector31 sP_G = {0,0,0};
};
VehiclePosition vehiclePosition;

struct CameraPosition
{
  float lat = PI/180.0 * -35.363182; // geodetic latitude [rad]
  float lon = PI/180.0 * 149.165234; // longitude [rad]
  float alt = 0; // altitude above mean sea level [m]
  Vector31 sC_G = {0,0,0}; 
};
CameraPosition cameraPosition;

struct CameraAngles
{
  float inclination;  // deg
  float azimuth;      // deg
};
CameraAngles cameraAngles;

// Function declarations
void computeVehiclePosition( VehiclePosition &vehiclePosition);
Vector31 computeRelativePosition_Geographic( VehiclePosition &vehiclePosition, CameraPosition &cameraPosition);
void extrapolatePosition( Vector31 &sPC_g, VehiclePosition &vehiclePosition);
void computeCameraAngles( CameraAngles &cameraAngles, Vector31 &sPC_g);

void setup() 
{
  // Light initialisation LED
  pinMode( 13, OUTPUT);
  digitalWrite( 13, HIGH);
  
  // Setup serial
  Serial.begin( 115200);
  

  // Setup SD card
  if ( !SD.begin( 4)) {
    Serial.println("Error opening SD card.");
  }

  // Un-comment this section if you want to read from the SD card
  /*
  while ( !Serial) {;}
  File f = SD.open( "angles.txt");
  while ( f.available()) {
    Serial.write( f.read());
  }
  f.close();
  */
  SD.remove( "angles.txt");
  
  // Initialise camera position
  const float N = a*a / ( sqrt( pow( a*cos( cameraPosition.lat), 2) + pow( b*sin( cameraPosition.lat), 2)));
  cameraPosition.sC_G = { ( N + cameraPosition.alt) * cos( cameraPosition.lat) * cos( cameraPosition.lon), 
                          ( N + cameraPosition.alt) * cos( cameraPosition.lat) * sin( cameraPosition.lon),
                          ( N*pow( 1-f, 2) + cameraPosition.alt) * sin( cameraPosition.lat)};

  // Finished initialisation
  digitalWrite( 13, LOW);
}

void loop() 
{
  static mavlink_message_t msg;
  static mavlink_status_t stat;
  static mavlink_global_position_int_t global_position_int_t;
  static char buf[512];
  static int loopCount = 0;

  // It's faster to read all in one go to clear the serial buffer
  Serial.readBytes( buf, 512);
  for ( char c : buf) {
    
    // Extract the MAVLink message
    if ( mavlink_parse_char( 0, c, &msg, &stat)) {
      // Global position message
      if ( msg.msgid == 33) {
        mavlink_msg_global_position_int_decode( &msg, &global_position_int_t);
        vehiclePosition.lat = global_position_int_t.lat / 1.0e7 * PI / 180;  
        vehiclePosition.lon = global_position_int_t.lon / 1.0e7 * PI / 180;  
        vehiclePosition.alt = global_position_int_t.alt / 1.0e3;
        vehiclePosition.vx = global_position_int_t.vx / 1.0e2;
        vehiclePosition.vy = global_position_int_t.vy / 1.0e2;
        vehiclePosition.vz = global_position_int_t.vz / 1.0e2;

        // Update position vector
        computeVehiclePosition( vehiclePosition);

        // Calculate the relative position vector in geographic coordinates
        Vector31 sPC_g = computeRelativePosition_Geographic( vehiclePosition, cameraPosition);

        // Use vehicle velocity to predict its position
        extrapolatePosition( sPC_g, vehiclePosition);

        // Calculate the angles required to aim at the vehicle
        computeCameraAngles( cameraAngles, sPC_g);

        break;
      }
    }
  }

  if ( loopCount++ % 10) {
    File dataFile = SD.open( "angles.txt", FILE_WRITE);
    dataFile.print( millis()/1000.0);
    dataFile.print( " ");
    dataFile.print( cameraAngles.inclination);
    dataFile.print( " ");
    dataFile.print( cameraAngles.azimuth);
    dataFile.println();
    dataFile.close();
  }
}

void computeVehiclePosition( VehiclePosition &vehiclePosition)
{
  const float N = a*a / ( sqrt( pow( a*cos( vehiclePosition.lat), 2) + pow( b*sin( vehiclePosition.lat), 2)));
  vehiclePosition.sP_G = { ( N + vehiclePosition.alt) * cos( vehiclePosition.lat) * cos( vehiclePosition.lon),
                           ( N + vehiclePosition.alt) * cos( vehiclePosition.lat) * sin( vehiclePosition.lon),
                           ( N*pow( 1-f, 2) + vehiclePosition.alt) * sin( vehiclePosition.lat)};
}

Vector31 computeRelativePosition_Geographic( VehiclePosition &vehiclePosition, CameraPosition &cameraPosition)
{
  Matrix33 T_gG = { -sin( cameraPosition.lat)*cos( cameraPosition.lon), -sin( cameraPosition.lat)*sin(cameraPosition.lon), cos( cameraPosition.lon),
                    -sin( cameraPosition.lon), cos( cameraPosition.lon), 0,
                    -cos( cameraPosition.lat)*cos( cameraPosition.lon), -cos( cameraPosition.lat)*sin(cameraPosition.lon), -sin( cameraPosition.lat)};
  Vector31 sPC_g = T_gG * ( vehiclePosition.sP_G - cameraPosition.sC_G);
  return sPC_g;
}

void extrapolatePosition( Vector31 &sPC_g, VehiclePosition &vehiclePosition)
{
  static uint64_t t = millis();
  uint32_t dt = millis() - t;
  if ( dt < 2000) {
    // Only extrapolate up to 2 s forward
    Vector31 Vb_g = { vehiclePosition.vx, vehiclePosition.vy, vehiclePosition.vz};
    sPC_g += ( Vb_g * dt) / 1000.0;
  }
}

void computeCameraAngles( CameraAngles &cameraAngles, Vector31 &sPC_g)
{
  cameraAngles.inclination = 180.0/PI * atan2( -sPC_g(2), sqrt( sPC_g(0)*sPC_g(0) + sPC_g(1)*sPC_g(1)));
  cameraAngles.azimuth = 180.0/PI * atan2( sPC_g(1), sPC_g(0));
}

