/*
 * Code for the album selection window. This window has First, previous, Next and Last buttons
 * to scroll through the list of albums. The album art and album name are displayed for the
  * currently selected album, and clicking on the album art image plays it.
  * This window also accepts steering wheel controls:
  * > Select next album
  * < Select previous album
  * O Play selected album
  * ^ Cancel selection
*/

#include "AlbumWindow.h"

AlbumWindow::AlbumWindow(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> &builder): Gtk::Window(cobject)
{
	signal_show().connect(sigc::mem_fun(*this, &AlbumWindow::onShow));
	builder->get_widget("AwAlbumNameLabel", albumLabel);
	builder->get_widget("AwAlbumImage", albumImage);
	builder->get_widget("AwMainEb", mainEb);
	builder->get_widget("AwAlbumEb", imageEb);

	mainEb->set_events(Gdk::BUTTON_PRESS_MASK);
	imageEb->set_events(Gdk::BUTTON_PRESS_MASK);
	mainEb->signal_button_press_event().connect(sigc::mem_fun(*this, &AlbumWindow::mouseClick));
	imageEb->signal_button_press_event().connect(sigc::mem_fun(*this, &AlbumWindow::imageClick));
}

void AlbumWindow::onShow()
{
	get_window()->set_cursor(Gdk::Cursor::create(Gdk::BLANK_CURSOR));
	albumIndex = player->GetCurrentAlbumIndex();
	SetAlbumControls();
}

void AlbumWindow::SetAlbumControls()
{
	// Sets the label and image based on the currently selected album
	const char* format = "<span foreground=\"#ffba00\" font=\"LCARS 16\">%s</span>";

	string name = player->GetAlbumName(albumIndex);
	albumLabel->set_markup(Glib::ustring(g_markup_printf_escaped(format, name.c_str())));

	string imagePath = player->GetAlbumArt(albumIndex);
	Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_file(imagePath, 160, 160, true);
	albumImage->set(pb);
}

bool AlbumWindow::imageClick(GdkEventButton* event)
{
	SelectAlbum();
}

void AlbumWindow::NextAlbum()
{
	unsigned int count = player->GetAlbumCount();
	
	if (albumIndex == (count - 1))
	{
		return;
	}

	albumIndex++;
	
	SetAlbumControls();
}

void AlbumWindow::PreviousAlbum()
{
	unsigned int count = player->GetAlbumCount();
	
	if (albumIndex == 0)
	{
		return;
	}

	albumIndex--;
	
	SetAlbumControls();
}

void AlbumWindow::SelectAlbum()
{
	player->SwitchAlbum(albumIndex);
	hide();
}

void AlbumWindow::Cancel()
{
	hide();
}

bool AlbumWindow::mouseClick(GdkEventButton* event)
{
	if (RectHit(nextButton, event->x, event->y))
	{
		NextAlbum();
	}

	if (RectHit(prevButton, event->x, event->y))
	{
		PreviousAlbum();
	}

	if (RectHit(firstButton, event->x, event->y))
	{
		albumIndex = 0;
		SetAlbumControls();
	}

	if (RectHit(lastButton, event->x, event->y))
	{
		unsigned int count = player->GetAlbumCount();
		albumIndex = count - 1;
		SetAlbumControls();
	}

	return true;
}
