/*
 *  The main window. Where all the fun stuff happens.
 */

#include "MainWindow.h"
#include <iomanip>
#include <string>
#include <ctime>
#include <gtkmm/window.h>
#include <libconfig.h++>

using namespace libconfig;

MainWindow::MainWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder): Gtk::Window(cobject)
{
	// Grab configs
	Config cfg;
	cfg.readFile("/etc/carputer/carputer.cfg");
	string musicPath = cfg.lookup("musicpath");
	string assetsp = cfg.lookup("assetspath");

	assetsPath = assetsp;

	// Objects
	player = new MusicPlayer(musicPath);
	gps = new GpsInfo();
	watcher = new GpioWatcher();

	// Get Widgets
	builder->get_widget("MainEb", mainEb);
	builder->get_widget("AlbumEb", albumEb);
	builder->get_widget("TrackLabel", titleLabel);
	builder->get_widget("ArtistLabel", artistLabel);
	builder->get_widget("AlbumImage", albumImage);
	builder->get_widget("TimeLabel", timerLabel);
	builder->get_widget_derived("AlbumWindow", albumWindow);
	builder->get_widget_derived("PostcodeWindow", postcodeWindow);
	builder->get_widget_derived("Speedo", speedo);

	albumWindow->player = player;

	// Connect signals/dispatchers/etc
	mainEb->signal_button_press_event().connect(sigc::mem_fun(*this, &MainWindow::mouseClick));
	albumEb->signal_button_press_event().connect(sigc::mem_fun(*this, &MainWindow::albumClick));
	speedo->signal_button_press_event().connect(sigc::mem_fun(*this, &MainWindow::navClick));
	signal_show().connect(sigc::mem_fun(*this, &MainWindow::load));
	player->signal_trackchanged().connect(sigc::mem_fun(*this, &MainWindow::trackChanged));
	player->signal_albumchanged().connect(sigc::mem_fun(*this, &MainWindow::albumChanged));
	gps->dataReady.connect(sigc::mem_fun(*this, &MainWindow::updateGps));
	
	// GPIO dispatchers
	watcher->prev_down.connect(sigc::mem_fun(*this, &MainWindow::prev_button_down));
	watcher->next_down.connect(sigc::mem_fun(*this, &MainWindow::next_button_down));
	watcher->mode_down.connect(sigc::mem_fun(*this, &MainWindow::mode_button_down));
	watcher->up_down.connect(sigc::mem_fun(*this, &MainWindow::up_button_down));

	// Timer to display the time
	sigc::slot<bool> timeSlot = sigc::mem_fun(*this, &MainWindow::onTimer);
	sigc::connection conn = Glib::signal_timeout().connect(timeSlot, 1000);
}

// Handler for '<' steering wheel control.
void MainWindow::prev_button_down()
{
	if (albumWindow->get_visible())
	{
		albumWindow->PreviousAlbum();
	}
	else if (postcodeWindow->get_visible())
	{
		postcodeWindow->PrevChar();
	}
	else
	{
		back();
	}
}

// Handler for '>' steering wheel control.
void MainWindow::next_button_down()
{
	if (albumWindow->get_visible())
	{
		albumWindow->NextAlbum();
	}
	else if (postcodeWindow->get_visible())
	{
		postcodeWindow->NextChar();
	}
	else
	{
		skip();
	}
}

// Handler for 'O' steering wheel control.
void MainWindow::mode_button_down()
{
	if (albumWindow->get_visible())
	{
		albumWindow->SelectAlbum();
	}
	else if (postcodeWindow->get_visible())
	{
		postcodeWindow->SelectChar();
	}
	else
	{
		albumWindow->show();
	}
}

// Handler for '^' steering wheel control.
void MainWindow::up_button_down()
{
	if (albumWindow->get_visible())
	{
		albumWindow->Cancel();
	}
	else if (postcodeWindow->get_visible())
	{
		postcodeWindow->Navigate();
	}
	else
	{
		postcodeWindow->Reset();
		postcodeWindow->show();
	}
}

// Callback for GPS watcher. Grabs the speed and hoofs it off to the speedo control
void MainWindow::updateGps()
{
	double lat, lng, speed;
	gps->getData(lat, lng, speed);

	speedo->SetSpeed(speed);
}

// Get the current time and display it in a label
bool MainWindow::onTimer()
{
	const char* format = "<span foreground=\"#000000\" font=\"LCARS 22\">%s</span>";
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	strftime(buffer, sizeof(buffer), "%T", timeinfo);

	timerLabel->set_markup(Glib::ustring(g_markup_printf_escaped(format, buffer)));
	return true;
}

// Displays the current album art
void MainWindow::albumChanged(int albumId)
{
	string imagePath = player->GetAlbumArt(albumId);
	Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_file(imagePath, 95, 95, true);
	albumImage->set(pb);
	albumIx = albumId;
}

// Displays current artist/title
void MainWindow::trackChanged(string artist, string title)
{
	const char* format = "<span foreground=\"#ffba00\" font=\"LCARS 16\">%s</span>";
	artistLabel->set_markup(Glib::ustring(g_markup_printf_escaped(format, artist.c_str())));
	titleLabel->set_markup(Glib::ustring(g_markup_printf_escaped(format, title.c_str())));
}

// Click handler - Pause and Skip buttons
bool MainWindow::mouseClick(GdkEventButton* event)
{
	if (RectHit(skipButton, event->x, event->y))
	{
		skip();
	}

	if (RectHit(pauseButton, event->x, event->y))
	{
		pause();
	}

	return true;
}

// Click handler - album art (show select album window)
bool MainWindow::albumClick(GdkEventButton* event)
{
	albumWindow->show();
	return true;
}

// Click handler - speedo (show navigation window)
bool MainWindow::navClick(GdkEventButton* event)
{
	postcodeWindow->Reset();
	postcodeWindow->show();
	return true;
}

void MainWindow::load()
{
	get_window()->set_cursor(Gdk::Cursor::create(Gdk::BLANK_CURSOR));
	player->Load();
	player->PlayRandomAlbum();
}

// Pause the current track and display a "Paused" image in place of the album art
void MainWindow::pause()
{
	bool paused = player->Pause();
	if (paused)
	{
		string imagePath = assetsPath + "/paused.png";
		Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_file(imagePath, 95, 95, true);
		albumImage->set(pb);
	}
	else
	{
		albumChanged(albumIx);
	}
}

void MainWindow::skip()
{
	player->Skip();
}

void MainWindow::back()
{
	player->Back();
}
