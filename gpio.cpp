/*
 *  This is a utility class to interface with the GPIO ports. I didn't write this library, but I tweaked
 *  it a little to work with bools instead of "1" and "0" strings.
  * Original code: http://hertaville.com/2012/11/18/introduction-to-accessing-the-raspberry-pis-gpio-in-c/
 */

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "gpio.h"
 
using namespace std;
 
GPIOClass::GPIOClass()
{
    this->gpionum = "4"; //GPIO4 is default
}
 
GPIOClass::GPIOClass(string gnum)
{
    this->gpionum = gnum;  //Instatiate GPIOClass object for GPIO pin number "gnum"
}
 
int GPIOClass::export_gpio()
{
	string export_str = "/sys/class/gpio/export";
	ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames
	
	if (exportgpio < 0)
	{
		cout << " OPERATION FAILED: Unable to export GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}
 
	exportgpio << this->gpionum ; //write GPIO number to export
	exportgpio.close(); //close export file
	return 0;
}
 
int GPIOClass::unexport_gpio()
{
	string unexport_str = "/sys/class/gpio/unexport";
	ofstream unexportgpio(unexport_str.c_str()); //Open unexport file
	
	if (unexportgpio < 0)
	{
		cout << " OPERATION FAILED: Unable to unexport GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}
 
	unexportgpio << this->gpionum ; //write GPIO number to unexport
	unexportgpio.close(); //close unexport file
	return 0;
}
 
int GPIOClass::setdir_gpio(string dir)
{
	string setdir_str ="/sys/class/gpio/gpio" + this->gpionum + "/direction";
	ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio
	
	if (setdirgpio < 0)
	{
		cout << " OPERATION FAILED: Unable to set direction of GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}

	setdirgpio << dir; //write direction to direction file
	setdirgpio.close(); // close direction file
	return 0;
}
 
int GPIOClass::setval_gpio(bool val)
{
	string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
	ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
	
	if (setvalgpio < 0)
	{
		cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}
 
	setvalgpio << (val ? "1" : "0") ;//write value to value file
	setvalgpio.close();// close value file 
	return 0;
}
 
int GPIOClass::getval_gpio(bool& val)
{
	string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
	ifstream getvalgpio(getval_str.c_str());// open value file for gpio
	if (getvalgpio < 0)
	{
		cout << " OPERATION FAILED: Unable to get value of GPIO"<< this->gpionum <<" ."<< endl;
		return -1;
	}

	string sval;
	getvalgpio >> sval ;  //read gpio value

	if(sval != "0")
	{
		val = true;
	}
	else
	{
		val = false;
	}

	getvalgpio.close(); //close the value file
	return 0;
}
 
string GPIOClass::get_gpionum()
{
	return this->gpionum;
}