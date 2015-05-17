/*
 *  This is a utility class that watches a set of GPIO pins and signals a Glib::Dispatcher when they go
 *  from LOW -> HIGH. This is for the steering wheel controls. There are six buttons on my steering wheel:
 *  +/- : Volume buttons (not hooked up here since they're wired directly into the amp)
 *  >   : Skip
 *  <   : Back
 *  ^   : Up (I use this one for nav)
 *  O   : Mode (I use this one for album select)
 *  The actual GPIO numbers for each are stored in the config so I can change the assignment later if I need to
 */

#include "GpioWatcher.h"
#include <libconfig.h++>
#include <unistd.h>

using namespace libconfig;

GpioWatcher::GpioWatcher()
	: thread(0)
{
	Config cfg;
	cfg.readFile("/etc/carputer/carputer.cfg");
	
	// Create a class to represent each button
	gpioPrev = new GPIOClass(cfg.lookup("gpio_prev_pin"));
	gpioNext = new GPIOClass(cfg.lookup("gpio_next_pin"));
	gpioMode = new GPIOClass(cfg.lookup("gpio_mode_pin"));
	gpioUp = new GPIOClass(cfg.lookup("gpio_up_pin"));
	
	// Export them all
	gpioPrev->export_gpio();
	gpioNext->export_gpio();
	gpioMode->export_gpio();
	gpioUp->export_gpio();

	// They're all inputs
    gpioPrev->setdir_gpio("in");
    gpioNext->setdir_gpio("in");
    gpioMode->setdir_gpio("in");
    gpioUp->setdir_gpio("in");

	// Kick off a loop in a new thread to watch them
	thread = Glib::Thread::create(sigc::mem_fun(*this, &GpioWatcher::watch), true);
}

void GpioWatcher::watch()
{
	bool gpioPrevState = false;
	bool gpioNextState = false;
	bool gpioModeState = false;
	bool gpioUpState = false;
	
	bool gpioPrevLastState = false;
	bool gpioNextLastState = false;
	bool gpioModeLastState = false;
	bool gpioUpLastState = false;
	
    while(true)
    {
		usleep(100000);  // wait for 0.1 seconds so we don't hammer the GPIO too much
		
		// Get the current values
		gpioPrev->getval_gpio(gpioPrevState);
		gpioNext->getval_gpio(gpioNextState);
		gpioMode->getval_gpio(gpioModeState);
		gpioUp->getval_gpio(gpioUpState);
		
		// Compare to previous values, see if they changed
		if (gpioPrevState && (gpioPrevState != gpioPrevLastState))
		{
			prev_down();
		}
		
		if (gpioNextState && (gpioNextState != gpioNextLastState))
		{
			next_down();
		}
		
		if (gpioModeState && (gpioModeState != gpioModeLastState))
		{
			mode_down();
		}
		
		if (gpioUpState && (gpioUpState != gpioUpLastState))
		{
			up_down();
		}
		
		// Preserve the last value for the next loop run
		gpioPrevLastState = gpioPrevState;
		gpioNextLastState = gpioNextState;
		gpioModeLastState = gpioModeState;
		gpioUpLastState = gpioUpState;
    }
}