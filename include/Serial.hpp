#pragma once

#include <iostream>
#include <string_view>

// C library headers
#include <iostream>

#include <cstdio>      // Standard input / output functions
#include <cstdlib>
#include <cstring>     // String function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <cerrno>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions


class Serial
{

public:

    Serial(std::string_view _port, std::size_t _baudRate)
    : port{_port}, baudRate{_baudRate}
    {
        handle = ::open(port.data(), O_RDWR);

        // Create new termios struct, we call it 'tty' for convention

        termios tty;

		if(tcgetattr(handle, &tty) != 0 )
		{
		   //std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
		}

		/* Save old tty parameters */

		/* Set Baud Rate */
		cfsetospeed(&tty, GetSpeedTFromBaudRate(baudRate));
		cfsetispeed(&tty, GetSpeedTFromBaudRate(baudRate));

		/* Setting other Port Stuff */
		tty.c_cflag &= 	~PARENB;            // Make 8n1
		tty.c_cflag &= 	~CSTOPB;
		tty.c_cflag &= 	~CSIZE;
		tty.c_cflag |= 	CS8;

		tty.c_cflag &= 	~CRTSCTS;           // no flow control
		tty.c_cc[VMIN] = 0;                  // read blocks
		tty.c_cc[VTIME] = 10;                  // 1 second read timeout
		tty.c_cflag |=	CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

		/* Make raw */
		cfmakeraw(&tty);

		/* Flush Port, then applies attributes */
		tcflush(this->handle, TCIFLUSH);
		if(tcsetattr (this->handle, TCSANOW, &tty) != 0)
		{
		   //std::cout << "Error " << errno << " from tcsetattr" << std::endl;
		}

    }

    ~Serial()
    {
        ::close(handle);
    }

    uint8_t ReadByte() const
    {
        uint8_t byte{};

        ::read(handle, &byte, 1);
        
        return byte;        
    }

private:

    static speed_t GetSpeedTFromBaudRate(std::size_t baudRate)
    {
        switch(baudRate)
        {
        case 115'200: return static_cast<speed_t>(B115200);
        
        default: return static_cast<speed_t>(B9600);
        }
    }

    std::string_view port;
    std::size_t baudRate;

    int handle{};

};
