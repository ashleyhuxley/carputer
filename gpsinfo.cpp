/*
 * This class watches the GPS and fires a dispatcher when there's new data. Mainly used for speedo.
 */
 
#include "gpsinfo.h"

using namespace std;

GpsInfo::GpsInfo()
	: thread(0)
{
	thread = Glib::Thread::create(sigc::mem_fun(*this, &GpsInfo::watch), true);
}

void GpsInfo::watch()
{
	// Start the GPS
	gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);

	if (gps_rec.stream(WATCH_ENABLE|WATCH_JSON) == NULL)
	{
		cout << "No GPSD running." << endl;
		return;
	}

	bool running = true;
	while (running)
	{
		struct gps_data_t* newdata;

		if (!gps_rec.waiting(50000000))
		{
			continue;
		}

		if ((newdata = gps_rec.read()) == NULL)
		{
			cout << "Read error." << endl;
			running = false;
		}
		else
		{
			{
				Glib::Mutex::Lock lock (mutex);
				latitude = newdata->fix.latitude;
				longitude = newdata->fix.longitude;
				speed = newdata->fix.speed;
			}

			dataReady();
		}
	}
}

// This function is called by the client object to get the data
void GpsInfo::getData(double& lat, double& lng, double& spd)
{
	{
		Glib::Mutex::Lock lock(mutex);
		lat = latitude;
		lng = longitude;
		spd = speed;
	}
}
