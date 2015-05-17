#ifndef ALBUMWINDOW_H
#define ALBUMWINDOW_H

#include <string>
#include <gtkmm/window.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>
#include <gtkmm/image.h>
#include <gtkmm/builder.h>
#include "musicplayer.h"
#include "rect.h"

class AlbumWindow : public Gtk::Window
{
	public:
		AlbumWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder);
		
		void NextAlbum();
		void PreviousAlbum();
		void SelectAlbum();
		void Cancel();

		MusicPlayer* player;

	private:
		void onShow();
		bool mouseClick(GdkEventButton* event);
		bool imageClick(GdkEventButton* event);
		void SetAlbumControls();

		Gtk::Label* albumLabel;
		Gtk::Image* albumImage;
		Gtk::EventBox* mainEb;
		Gtk::EventBox* imageEb;
		rect prevButton = {10, 40, 60, 74};
		rect firstButton = {10, 126, 60, 74};
		rect nextButton = {250, 40, 60, 74};
		rect lastButton = {250, 126, 60, 74};
		unsigned int albumIndex;
};

#endif
