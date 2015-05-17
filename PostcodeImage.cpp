/*
 *  Image that represents the postcode in the PostcodeWindow.
 */

#include "PostcodeImage.h"
#include <cairomm/context.h>
#include <iostream>
#include <sstream>

using namespace std;

inline string toString(char c)
{
	string s;
	stringstream ss;
	ss << c;
	ss >> s;
	return s;
}

PostcodeImage::PostcodeImage(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder): Gtk::DrawingArea(cobject)
{
}

PostcodeImage::~PostcodeImage()
{
}

bool PostcodeImage::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->set_source_rgb(1, 0.6, 0);

	// Draw the cursor. Might not be visible because blinking.
	if (cursorVisible)
	{
		cr->set_line_width(3.0);

		cr->move_to(10 + (cursorPos * 45), 48);
		cr->line_to(40 + (cursorPos * 45), 48);

		cr->stroke();
	}
	
	Pango::FontDescription font;
	font.set_family("LCARS");
	font.set_absolute_size(1024 * 40);

	// Draw each avalable character
	for (int i = 0; i < postcode.length(); i++)
	{
		if (postcode[i] != 0)
		{
			Glib::RefPtr<Pango::Layout> layout = create_pango_layout(toString(postcode[i]));
			layout->set_font_description(font);
			cr->move_to(16 + (i * 45), 3);
			layout->show_in_cairo_context(cr);
		}
	}
	
	return true;
}

void PostcodeImage::blink()
{
	cursorVisible = !cursorVisible;
	queue_draw();
}

void PostcodeImage::update(string pc, unsigned int cp)
{
	postcode = pc;
	cursorPos = cp;
	queue_draw();
}
