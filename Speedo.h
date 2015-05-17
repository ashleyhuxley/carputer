#ifndef SPEEDO_H
#define SPEEDO_H

#include <gtkmm/builder.h>
#include <gtkmm/drawingarea.h>

class Speedo : public Gtk::DrawingArea
{
	public:
		Speedo(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder);
		virtual ~Speedo();

		void SetSpeed(double speedMps);

	protected:
		virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

		double speed = 0;
};

#endif
