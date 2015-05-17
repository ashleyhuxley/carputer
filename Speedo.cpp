/*
 *  This is the speedo control on the main window. Takes a speed and draws an arc
 *  with an angle dependant on the speed. Also displays speed in mph.
 *  Actually it's two arcs - one blue, one purple.
 */

#include "Speedo.h"
#include <cairomm/context.h>
#include <cmath>

using namespace std;

Speedo::Speedo(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder): Gtk::DrawingArea(cobject)
{
}

Speedo::~Speedo()
{
}

bool Speedo::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// GPS speed in in meters/sec so convert
	double speedMph = speed * 2.2369;
	
	// Angle is in radians. 0.018 is 1% of the available angle, so we have a range of 0-100mph
	double angle = (speedMph * 0.018) + 0.6;
	
	// Convert to int for display
	int speedInt = static_cast<int>(speedMph + 0.5);

	// Display the arcs
	cr->set_line_width(17.0);

	cr->arc(48, 48, 35, (0.6 * M_PI), angle * M_PI);
	cr->set_source_rgb(0.39, 0.6, 1);
	cr->stroke();

	cr->arc(48, 48, 35, (angle) * M_PI, (2.4 * M_PI));
	cr->set_source_rgb(0.67, 0.458, 1);
	cr->stroke();

	// Draw the speed as text
	Pango::FontDescription font;
	font.set_family("LCARS");
	font.set_absolute_size(1024 * 30);

	Glib::RefPtr<Pango::Layout> layout = create_pango_layout(to_string(speedInt));
	layout->set_font_description(font);

	int w, h;
	layout->get_pixel_size(w, h);

	cr->set_source_rgb(1, 0.6, 0);
	cr->move_to((95 - w) / 2, (95 - h) / 2);
	layout->show_in_cairo_context(cr);

	return true;
}

void Speedo::SetSpeed(double speedMps)
{
	if (std::isnan(speedMps))
	{
		return;
	}

	speed = speedMps;
	queue_draw();
}
