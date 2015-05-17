#ifndef POSTCODEIMAGE_H
#define POSTCODEIMAGE_H

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>
#include <string>

using namespace std;

class PostcodeImage : public Gtk::DrawingArea
{
	public:
		PostcodeImage(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder);
		virtual ~PostcodeImage();

		void blink();
		void update(string pc, unsigned int cp);

	protected:
		virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
		bool cursorVisible = false;
		unsigned int cursorPos = 0;
		string postcode;
};

#endif
