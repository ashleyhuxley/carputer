/* Code for the Postcode Selection window. When the user selects Navigate, they are presented with this
 * window. It has a simple keypad (A-Z, 0-9), to enter a postcode. Once complete, the postcode is looked
 * up in a SqLite database for the Lat/Lng coords which are passed via DBus to Navit.
 * The window also accepts steering wheel controls:
 * > Select next character
 * < Select previous character
 * O Accep this character and advnace cursor
 * ^ Accept entry and navigate to postcode
 */

#include "PostcodeWindow.h"
#include <iostream>
#include <string>
#include <glibmm/main.h>
#include <sqlite3.h>
#include <sstream>
#include <libconfig.h++>
#include <stdlib.h>

using namespace std;
using namespace libconfig;

PostcodeWindow::PostcodeWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder): Gtk::Window(cobject)
{
	// Main event box to receive events, and image to show the postcode and flashing cursor
	builder->get_widget("PwMainEb", mainEb);
	builder->get_widget_derived("PostcodeDrawingArea", postcodeImg);

	// Events
	mainEb->set_events(Gdk::BUTTON_PRESS_MASK);
	postcodeImg->set_events(Gdk::BUTTON_PRESS_MASK);
	mainEb->signal_button_press_event().connect(sigc::mem_fun(*this, &PostcodeWindow::mouseClick));
	postcodeImg->signal_button_press_event().connect(sigc::mem_fun(*this, &PostcodeWindow::postcodeImgClick));
	signal_show().connect(sigc::mem_fun(*this, &PostcodeWindow::onShow));

	// Timer to flash the cursor
	sigc::slot<bool> timerSlot = sigc::mem_fun(*this, &PostcodeWindow::onTimer);
	sigc::connection conn = Glib::signal_timeout().connect(timerSlot, 500);
}

void PostcodeWindow::onShow()
{
	get_window()->set_cursor(Gdk::Cursor::create(Gdk::BLANK_CURSOR));
}

bool PostcodeWindow::onTimer()
{
	postcodeImg->blink();
	return true;
}

void PostcodeWindow::NextChar()
{
	// Steering wheel control to select next character
	char& c = postcode[cursorPos];
	if (c == 0)
	{
		c = 'A';
	}
	else
	{
		c++;
		if (c == 58)
		{
			c = 65;
		}
		if (c == 91)
		{
			c = 48;
		}
	}
	
	update();
}

void PostcodeWindow::PrevChar()
{
	// Steering wheel control to select previous character
	char& c = postcode[cursorPos];
	if (c == 0)
	{
		c = '9';
	}
	else
	{
		c--;
		if (c == 47)
		{
			c = 90;
		}
		if (c == 64)
		{
			c = 57;
		}
	}
	
	update();
}

void PostcodeWindow::SelectChar()
{
	// Accept selected character and advance cursor
	if (cursorPos < 6)
	{
		cursorPos++;
	}
	else
	{
		cursorPos = 0;
	}
	
	update();
}

void PostcodeWindow::Navigate()
{
	// Navigate to selected postcode by passing lat/lng to Navit via Dbus
	string pc(postcode);
	
	string lat, lng;
	getLatLng(pc, lat, lng);
	
	stringstream dbusCmd;
	dbusCmd << "dbus-send  --print-reply --session --dest=org.navit_project.navit /org/navit_project/navit/default_navit org.navit_project.navit.navit.set_destination string:\"geo: " << lng << " " << lat << "\" string:\"" << pc << "\"";
	
	// I'd rather do this using the GDbus library but the documentation makes it look complicated. Maybe later.
	system("navit");
	system(dbusCmd.str().c_str());
	system("dbus-send  --print-reply --session --dest=org.navit_project.navit /org/navit_project/navit/default_navit org.navit_project.navit.navit.draw");
	
	hide();
}

void PostcodeWindow::update()
{
	// Forces an update on the postcode image
	string pc(postcode);
	postcodeImg->update(pc, cursorPos);
}

void PostcodeWindow::Reset()
{
	// Remove the entered postcode and set cursor to start
	cursorPos = 0;

	for (int i = 0 ; i <= 7; i++)
	{
		postcode[i] = '\0';
	}
	
	update();
}

bool PostcodeWindow::postcodeImgClick(GdkEventButton* event)
{
	// User can select cursor position by tapping the character
	if (event->x < 46) { cursorPos = 0; }
	else if (event->x < 91) { cursorPos = 1; }
	else if (event->x < 137) { cursorPos = 2; }
	else if (event->x < 183) { cursorPos = 3; }
	else if (event->x < 228) { cursorPos = 4; }
	else if (event->x < 274) { cursorPos = 5; }
	else if (event->x < 320) { cursorPos = 6; }
	
	update();
}

bool PostcodeWindow::mouseClick(GdkEventButton* event)
{
	// This is the main keypad handler
	char button = getButton(event->x, event->y);

	if (button != 0)
	{
		// Cancel button was pressed. Do nothing.
		if (button == '<')
		{
			hide();
			return true;
		}

		// Go button pressed - navigate to entry
		if (button == '>')
		{
			Navigate();
			return true;
		}

		// Alphanumeric button pressed
		postcode[cursorPos] = button;
		if (cursorPos < 6)
		{
			cursorPos++;
		}
		
		update();
	}
}

char PostcodeWindow::getButton(gdouble x, gdouble y)
{
	// Takes a coordinate and returns the button at that coordinate.
	// Returns 0 if no button pressed

	if (y < 103) { return 0; }

	if (y < 135)
	{
		if (x < 10) { return 0; }
		if (x < 40) { return '1'; }
		if (x < 70) { return '2'; }
		if (x < 100) { return '3'; }
		if (x < 130) { return '4'; }
		if (x < 160) { return '5'; }
		if (x < 190) { return '6'; }
		if (x < 220) { return '7'; }
		if (x < 250) { return '8'; }
		if (x < 280) { return '9'; }
		if (x < 310) { return '0'; }
		return 0;
	}

	if (y < 167)
	{
		if (x < 10) { return 0; }
		if (x < 40) { return 'Q'; }
		if (x < 70) { return 'W'; }
		if (x < 100) { return 'E'; }
		if (x < 130) { return 'R'; }
		if (x < 160) { return 'T'; }
		if (x < 190) { return 'Y'; }
		if (x < 220) { return 'U'; }
		if (x < 250) { return 'I'; }
		if (x < 280) { return 'O'; }
		if (x < 310) { return 'P'; }
		return 0;
	}

	if (y < 199)
	{
		if (x < 25) { return 0; }
		if (x < 55) { return 'A'; }
		if (x < 85) { return 'S'; }
		if (x < 115) { return 'D'; }
		if (x < 145) { return 'F'; }
		if (x < 175) { return 'G'; }
		if (x < 205) { return 'H'; }
		if (x < 235) { return 'J'; }
		if (x < 265) { return 'K'; }
		if (x < 295) { return 'L'; }
		return 0;	
	}

	if (y < 231)
	{
		if (x < 10) { return 0; }
		if (x < 40) { return '<'; }
		if (x < 70) { return 'Z'; }
		if (x < 100) { return 'X'; }
		if (x < 130) { return 'C'; }
		if (x < 160) { return 'V'; }
		if (x < 190) { return 'B'; }
		if (x < 220) { return 'N'; }
		if (x < 250) { return 'M'; }
		if (x < 310) { return '>'; }
		return 0;
	}

	return 0;
}

int PostcodeWindow::getLatLng(string postcode, string& lat, string& lng)
{
	// Looks up the postcode from a SqLite database.
	// SqLite db has a simple table - Postcode, with three fields - Postcode, lat, Lng.
	// It's a simple lookup but there are just shy of 1.7 million postcodes...
	
	sqlite3* database;
	
	Config cfg;
	cfg.readFile("/etc/carputer/carputer.cfg");
	string dbPath = cfg.lookup("postcodedb");

	if(sqlite3_open(dbPath.c_str(), &database) != SQLITE_OK)
	{
		cout << "Could not open database" << endl;
		return 1;
	}
	
	sqlite3_stmt* statement;

	stringstream sqlstr;
	sqlstr << "SELECT * FROM Postcode WHERE Postcode = '" << postcode << "';";
	
	if(sqlite3_prepare_v2(database, sqlstr.str().c_str(), -1, &statement, 0) != SQLITE_OK)
	{
		cout << "Prepare statement failed" << endl;
		return 1;
	}

	int result = 0;
	result = sqlite3_step(statement);
	 
	if(result == SQLITE_ROW)
	{
		char* latn = (char*)sqlite3_column_text(statement, 1);
		char* lngn = (char*)sqlite3_column_text(statement, 2);
		
		lat = string((char*)(latn ? latn : ""));
		lng = string((char*)(lngn ? lngn : ""));
	}
	
	sqlite3_finalize(statement);

	string error = sqlite3_errmsg(database);
	if(error != "not an error")
	{
		cout << error << endl;
		return 1;
	}
	
	return 0;
}
