#ifndef GPSINFO_H
#define GPSINFO_H

#include <libgpsmm.h>
#include <glibmm/dispatcher.h>
#include <glibmm/thread.h>
#include <iostream>

class GpsInfo
{
	public:
		GpsInfo();
		Glib::Dispatcher dataReady;

		void getData(double& lat, double& lng, double& speed);

	protected:
		Glib::Thread *thread;
		void watch();
		Glib::Mutex mutex;
		double latitude, longitude, speed;
};

#endif
