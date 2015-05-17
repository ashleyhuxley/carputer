#ifndef POSTCODEWINDOW_H
#define POSTCODEWINDOW_H

#include <string>
#include <gtkmm/window.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/builder.h>
#include "PostcodeImage.h"

class PostcodeWindow : public Gtk::Window
{
	public:
		PostcodeWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder);
		void Reset();
		
		void NextChar();
		void PrevChar();
		void SelectChar();
		void Navigate();

	private:
		bool mouseClick(GdkEventButton* event);
		bool postcodeImgClick(GdkEventButton* event);
		char getButton(gdouble x, gdouble y);
		bool onTimer();
		int getLatLng(string postcode, string& lat, string& lng);
		void update();
		void onShow();

		Gtk::EventBox* mainEb;
		unsigned int cursorPos = 0;
		char postcode[8];
		PostcodeImage* postcodeImg;
};

#endif
