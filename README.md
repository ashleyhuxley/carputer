# carputer
Gtkmm in-car computer for rasperry pi

##DEPENDENCIES

Before this code can be compiled, there are a number of dependencies that must be installed:

Libraries required by Carputer:
	sudo apt-get install libmpg123-dev					# MP3 library
	sudo apt-get install libao-dev						# Sound driver library
	sudo apt-get install libconfig++-dev				# For reading config files
	sudo apt-get install libgtkmm-3.0-dev				# C++ wrapper around GTK (for user interface)
	sudo apt-get install libsqlite3-dev					# SQLite - For reading postcode database

GPSD must be compiled from source. Using apt-get will not install the library:
	wget http://download.savannah.gnu.org/releases/gpsd/gpsd-3.11.tar.gz
	tar -xzf gpsd-3.11.tar.gz
	cd gpsd-3.11

Install scons (required to build gpsd):
	sudo apt-get install scons

Install libraries required by gpsd:
	sudo apt-get install libncurses-dev
	sudo apt-get install python-dev

Build gpsd:
	scons
	scons check
	sudo scons udev-install
	
You also need to upgrade GCC to the latest version so that you can use the new C++ 11 standard:
	sudo apt-get install gcc-4.7 g++-4.7
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.6
	sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 40 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7

Then run this command and select gcc-4.7.
	sudo update-alternatives --config gcc

You should then be able to compile the code just by typing:

make



##ABOUT THE PROJECT

Quite a lot of this is custom code written to my tight specifications so everything has to be done in a certain way. It's not written to be highly configurable - I wrote it with myself and my own requirements in mind with no plans to make it a releasable project so you'll have to bear with me for those bits :)

The bulk of this code is a music player but it has some GPS functionality - a speedo and a postcode link to Navit.

The assets directory contains the graphics for the application. You can edit these - but if you move the buttons around you'll have to change the corresponding coordinates in the code files.

The file 'carputer.glade' is the UI of the application. it is loaded at runtime and is used to build the interface, along with the graphics. It's XML so you can edit it by hand or using the Glade editor ("sudo apt-get install glade", I think)

carputer.cfg contains the configuration:

musicpath - Path to the directory where the MP3s are (more on this later)
assetspath - Path to the directory containing carputer.glade and the graphics (I keep them in /etc/carputer)
postcodedb - This is for postcode lookup - a SQLite db containing a Postcode to Lat/Lng lookup table. It's 75Mb so I haven't included it. Let me know if you want it.

The GPIO pins are for the steering wheel controls. There are four buttons that control the application: Next, previous, navigate and mode. The pins for those are configured here.

The music directory has to be laid out in a specific way. You can't just point it to a directory of MP3s and expect it to work... it won't.

Each directory within the main music directory is treated as one album, named in the format "Artist - Album Name".
Any files in the music dir root are ignored.

Only MP3 files within the album directories are loaded. (It doesn't support any other format). No sub-directories are loaded. MP3s should be named starting with the track number so they are loaded in order; "01 Track name.mp3". The actual artist/track name displayed in the UI is taken from the ID3 tags.

There *must* also be a 160x160px PNG file called albumart.png in the album directory. I'm a bit anal about my album art.

Anything else in the album dir is ignored.

So for example if your root music dir is /media/PENDRIVE, your layout would look like this:

/media/PENDRIVE/Pink Floyd - Dark Side of the Moon/01 - Speak to Me.mp3
/media/PENDRIVE/Pink Floyd - Dark Side of the Moon/02 - Breathe.mp3
...
/media/PENDRIVE/Pink Floyd - Dark Side of the Moon/10 - Eclipse.mp3
/media/PENDRIVE/Pink Floyd - Dark Side of the Moon/albumart.png
/media/PENDRIVE/Michael Jackson - Thriller/01 - Wanna Be Startin Somethin.mp3
/media/PENDRIVE/Michael Jackson - Thriller/02 - Baby Be Mine.mp3
...
/media/PENDRIVE/Michael Jackson - Thriller/09 - The Lady In My Life.mp3
/media/PENDRIVE/Michael Jackson - Thriller/albumart.png


##CONSIDERATIONS

This is my first ever C++ application so bear that in mind. (I'm a C# developer by trade). There might be a whole load of bugs/memory leaks.

The link to Navit doesn't quite work yet - I think I need a delay between starting up Navit and passing it the DBUS signals.

The Postcode Lookup screen/functionality is useless if you're outside the UK. You could just bypass it by doing:

	system("navit");

when the speedo is clicked, rather than have it open the PostcodeWindow.

This code is designed to run on a 320x240 touch screen - it hides the mouse pointer. If you want to test the code *with* the pointer, comment out the line:

	get_window()->set_cursor(Gdk::Cursor::create(Gdk::BLANK_CURSOR));

in the .cpp files for each window (MainWindow.cpp, AlbumWindow.cpp, PostcodeWindow.cpp).

If you want a title bar, in the Glade file, set "decorated" to "True" for each window. (No need to recompile - the Glade file is loaded at runtime)