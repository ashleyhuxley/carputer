#include <iostream>
#include <string>
#include <glibmm/fileutils.h>
#include <glibmm/markup.h>
#include <libconfig.h++>
#include "MainWindow.h"

using namespace std;
using namespace libconfig;

int main(int argc, char **argv)
{
	if (!Glib::thread_supported())
	{
		Glib::thread_init();
	}

	srand(time(0));

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.huxley.carputer");
	Glib::RefPtr<Gtk::Builder> builder;

	Config cfg;
	cfg.readFile("/etc/carputer/carputer.cfg");
	string assetsPath = cfg.lookup("assetspath");

	try
	{
		// The interface is built using an XML file from Glade
		 builder = Gtk::Builder::create_from_file(assetsPath + "/carputer.glade");
	}
	catch (const Glib::FileError &ex)
	{
		cerr << "File Error: " << ex.what() << endl;
		return 1;
	}
	catch (const Glib::MarkupError &ex)
	{
		cerr << "Markup Error: " << ex.what() << endl;
		return 1;
	}
	catch (const Gtk::BuilderError &ex)
	{
		cerr << "Builder Error: " << ex.what() << endl;
		return 1;
	}

	// Get the main window from the Glade file and enter the main Gtk loop
	MainWindow* win = 0;
	builder->get_widget_derived("MainWindow", win);
	return app->run(*win);
}
