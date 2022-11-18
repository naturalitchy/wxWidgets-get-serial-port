#ifndef get_serial_port
#define get_serial_port

#include "static-variable.h"

//getSerialPort function include
#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

//getSerialPort function include end
class GetSerialPort {
	private:
		//Serial Com port name
		std::string portName;
	public:
		GetSerialPort();
		~GetSerialPort();
		//Get Serial Communication Com port.
		int getCOMPort();
};


#endif