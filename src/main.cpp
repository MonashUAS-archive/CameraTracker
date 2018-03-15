// Boost includes
#include <boost/asio.hpp>

// Standard library includes
#include <iostream>
#include <signal.h>
#include <thread>
#include <math.h>

// MAVLink include
#include "../include/c_library_v2/common/mavlink.h"

// Local includes
#include "config.h"
#include "aircraft.h"

// Function declarations
void handleSigInt(int a);
void processMavLink();

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
    // serialPort.open( "ACM0");
  }
  catch (...) {
    std::cerr << "\033[1;31m"
              << "Port ACM0 not available. Check that the Arduino is connected.\n"
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

  // Calculate the required inclination and azimuth angles
  

  // Clean up
  serialPort.close();
  udpSocket.close();
  return 0;
}

void handleSigInt(int a)
{
  // Release the ports on SIGINT and rejoin read thread
  std::cerr << "Releasing serial and UDP ports.\n";
  sigIntCaught = true;
  serialPort.close();
  udpSocket.close();
  exit(1);
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
  }
}
