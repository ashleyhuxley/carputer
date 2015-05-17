#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <gtkmm/window.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/builder.h>
#include "musicplayer.h"
#include "AlbumWindow.h"
#include "rect.h"
#include "gpsinfo.h"
#include "Speedo.h"
#include "PostcodeWindow.h"
#include "GpioWatcher.h"

using namespace std;

class MainWindow : public Gtk::Window
{
	public:
		MainWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder);

	private:
		bool onTimer();
		void albumChanged(int albumId);
		void trackChanged(string artist, string title);
		bool mouseClick(GdkEventButton* event);
		bool albumClick(GdkEventButton* event);
		bool navClick(GdkEventButton* event);
		void load();
		void pause();
		void skip();
		void back();
		void updateGps();
		void prev_button_down();
		void next_button_down();
		void mode_button_down();
		void up_button_down();

		MusicPlayer *player;
		GpsInfo *gps;
		rect skipButton = {10, 60, 90, 40};
		rect pauseButton = {10, 110, 90, 40};
		rect albumButton = {110, 40, 95, 95};
		Gtk::EventBox* mainEb;
		Gtk::EventBox* albumEb;
		Gtk::EventBox* navEb;
		Gtk::Label* artistLabel;
		Gtk::Label* titleLabel;
		Gtk::Label* timerLabel;
		Gtk::Image* albumImage;
		Speedo* speedo;
		AlbumWindow* albumWindow;
		PostcodeWindow* postcodeWindow;
		int albumIx;
		string assetsPath;
		GpioWatcher* watcher;
};

#endif
