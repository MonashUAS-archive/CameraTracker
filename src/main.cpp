// Boost includes
#include <boost/asio.hpp>

// Standard library includes
#include <iostream>
#include <signal.h>

// Local includes
#include "config.h"

// Function declarations
void handleSigInt(int a);

// Global connections
boost::asio::io_service io;
boost::asio::serial_port serialPort( io);
boost::asio::ip::udp::socket udpSocket( io);

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
  }

  // Release the serial port
  serialPort.close();
  udpSocket.close();
  return 0;
}

void handleSigInt(int a)
{
  // Release the ports on SIGINT
  std::cerr << "Releasing serial and UDP ports.\n";
  serialPort.close();
  udpSocket.close();
  exit(1);
}
