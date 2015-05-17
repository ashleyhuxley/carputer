#ifndef GPIOWATCHER_H
#define GPIOWATCHER_H

#include "gpio.h"
#include <glibmm/dispatcher.h>
#include <glibmm/thread.h>

class GpioWatcher
{
	public:
		GpioWatcher();

		Glib::Dispatcher prev_down;
		Glib::Dispatcher next_down;
		Glib::Dispatcher mode_down;
		Glib::Dispatcher up_down;

	protected:
		void watch();
	
		Glib::Thread *thread;
		GPIOClass* gpioPrev;
		GPIOClass* gpioNext;
		GPIOClass* gpioMode;
		GPIOClass* gpioUp;

};

#endif