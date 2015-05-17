#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <ao/ao.h>
#include <mpg123.h>
#include <glibmm/dispatcher.h>
#include <glibmm/thread.h>

#define BITS 8

using namespace std;

typedef struct str_mp3id
{
	string artist;
	string title;
} mp3id;

class Player
{
	public:
		Player(): thread(0) 
		{ }

		~Player()
		{
			Stop();
		}

		void Play(string file);
		bool Pause();
		void Stop();
		mp3id GetTrackId();

		Glib::Dispatcher playDone;
		Glib::Dispatcher trackChanged;

	protected:
		void play();

		Glib::Thread *thread;
		Glib::Mutex mutex;
		string fileName;
		volatile bool paused;
		volatile bool playing;
		string artist, title;
};

#endif
