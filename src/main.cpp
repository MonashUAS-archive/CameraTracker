// Boost includes
#include <boost/asio.hpp>

// Standard library includes
#include <iostream>
#include <signal.h>
#include <thread>
#include <math.h>
#include <iomanip>

// MAVLink include
#include "../include/c_library_v2/common/mavlink.h"

// Armadillo include
#include <armadillo>

// Local includes
#include "config.h"
#include "aircraft.h"

// Function declarations
void handleSigInt(int a);
void processMavLink();
void sendArduinoData( double &inclination, double &azimuth);

// Global variables (sorry-not-sorry)
boost::asio::io_service io;
boost::asio::serial_port serialPort( io);
boost::asio::ip::udp::socket udpSocket( io);
std::thread mavlinkThread;
bool sigIntCaught = false;
char buf[300];
Aircraft aircraft;

int main(int argc, char **argv)
{
  // Attach SIGINT handler
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = handleSigInt;
  sigemptyset( &sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction( SIGINT, &sigIntHandler, NULL);

  // Handle configuration
  Config config( "../config.conf");

  // Bind to the Arduino's serial port
  try {
    serialPort.open( "/dev/" + config.serialPort);
  }
  catch (...) {
    std::cerr << "\033[1;31m"
              << "Port " << config.serialPort << " not available. Check that the Arduino is connected.\n"
              << "\033[0m";
    return 1;
  }

  // Bind to a UDP port to receive MAVLink packets
  udpSocket.open( boost::asio::ip::udp::v4());
  try {
    udpSocket.bind( boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( "127.0.0.1"), config.udpPort));
  }
  catch (...) {
    std::cerr << "\033[1;31m"
              << "Port " << config.udpPort << " not available.\n"
              << "\033[0m";
    return 1;
  }

  // Start the MAVLink read thread
  mavlinkThread = std::thread( processMavLink);

  //// Calculate the required inclination and azimuth angles
  while ( true) {
    // Constants
    const double a = 6378137.0;
    const double f = 1/298.257223563;
    const double b = a*(1-f);

    // Camera position
    const double cameraN = a*a / ( sqrt( pow( a*cos( config.latitude), 2) + pow( b*sin( config.latitude), 2)));
    const arma::vec3 sC_G( {( cameraN + config.altitudeAmsl) * cos( config.latitude) * cos( config.longitude),
                            ( cameraN + config.altitudeAmsl) * cos( config.latitude) * sin( config.longitude),
                            ( cameraN*pow( 1-f, 2) + config.altitudeAmsl) * sin( config.latitude)});

    // Aircraft position
    double aircraftN = a*a / ( sqrt( pow( a*cos( aircraft.latitude), 2) + pow( b*sin( aircraft.latitude), 2)));
    arma::vec3 sP_G( {( aircraftN + aircraft.altitude) * cos( aircraft.latitude) * cos( aircraft.longitude),
                      ( aircraftN + aircraft.altitude) * cos( aircraft.latitude) * sin( aircraft.longitude),
                      ( aircraftN*pow( 1-f, 2) + aircraft.altitude) * sin( aircraft.latitude)});

    // Transform to geographic coordinates
    const arma::mat33 T_gG( {{-sin( config.latitude)*cos( config.longitude), -sin( config.latitude)*sin(config.longitude), cos( config.longitude)},
                             {-sin( config.longitude), cos( config.longitude), 0},
                             {-cos( config.latitude)*cos( config.longitude), -cos( config.latitude)*sin(config.longitude), -sin( config.latitude)}});
    arma::vec3 sPC_g = T_gG * ( sP_G - sC_G);
    if ( config.latitude < 0) {
      // sPC_g[0] *= -1;
    }

    // std::cout << sPC_g << "\n";
    // std::cout << config.altitudeAmsl << "  " << aircraft.altitude << "\n";

    // Calculate angles in degrees
    double inclination = 180/M_PI * atan2( -sPC_g[2], sqrt( sPC_g[0]*sPC_g[0] + sPC_g[1]*sPC_g[1]));
    double azimuth = 180/M_PI * ( atan2( sPC_g[1], sPC_g[0]));
    std::cout << inclination << "  " << azimuth << std::endl;
    usleep(1e6/10);

    // Send data to the Arduino
    sendArduinoData( inclination, azimuth);
  }

  // Clean up
  serialPort.close();
  udpSocket.close();
  return 0;
}

void handleSigInt(int a)
{
  // Release the ports on SIGINT and rejoin read thread
  std::cerr << "\nReleasing serial and UDP ports.\n";
  sigIntCaught = true;
  serialPort.close();
  udpSocket.close();
  // Quick exit to play nice with multithreading
  quick_exit(0);
}

void processMavLink()
{
  while ( !sigIntCaught) {
    if ( udpSocket.available()) {
      udpSocket.receive( boost::asio::buffer( buf));

      mavlink_message_t msg;
      mavlink_status_t status;
      for (char c : buf) {
        if ( mavlink_parse_char( 1, c, &msg, &status)) {
          break;
        }
      }
      // Global position int message
      if ( msg.msgid == 33) {
        mavlink_global_position_int_t globalPosition;
        mavlink_msg_global_position_int_decode( &msg, &globalPosition);
        aircraft.latitude =      globalPosition.lat / 1.0e7 * M_PI / 180;
        aircraft.longitude =     globalPosition.lon / 1.0e7 * M_PI / 180;
        aircraft.altitude =      globalPosition.alt / 1.0e3;
        aircraft.velocityBodyX = globalPosition.vx / 1.0e2;
        aircraft.velocityBodyY = globalPosition.vy / 1.0e2;
        aircraft.velocityBodyZ = globalPosition.vz / 1.0e2;
      }
    }
    usleep(10e3); // 10 ms sleep to not overload the thread
  }
}

void sendArduinoData( double &inclination, double &azimuth)
{
  try {
    std::string arduinoString = std::to_string( inclination) + " " + std::to_string( azimuth);
    serialPort.write_some( boost::asio::buffer( arduinoString));
  }
  catch (...) {
    // The write operation failed - end the program
    std::cerr << "\033[1;31m"
              << "Arduino no longer detected.\n"
              << "\033[0m";
    handleSigInt( 0);
  }
}