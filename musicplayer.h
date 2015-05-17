#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <vector>
#include <string>
#include <time.h>
#include "player.h"

using namespace std;

// Structure to hold the album name and names of all the tracks.
typedef struct str_album
{
	string name;
	vector<string> tracks;
} album;

typedef sigc::signal<void, string, string> type_signal_trackchanged;
typedef sigc::signal<void, int> type_signal_albumchanged;

class MusicPlayer
{
	public:
		MusicPlayer(string musicDir);

		~MusicPlayer()
		{
			mpg123_exit();
			ao_shutdown();
		}

		void Load();
		bool Pause();
		void Skip();
		void Back();
		void PlayRandomAlbum();

		type_signal_trackchanged signal_trackchanged();
		type_signal_albumchanged signal_albumchanged();
		string GetAlbumArt(int albumId);
		unsigned int GetCurrentAlbumIndex();
		string GetAlbumName(unsigned int idx);
		unsigned int GetAlbumCount();
		void SwitchAlbum(int index);

	protected:
		type_signal_trackchanged m_signal_trackchanged;
		type_signal_albumchanged m_signal_albumchanged;

	private:
		void trackChanged();
		void loadAlbums();
		string ResolveTrackPath(int albumIx, int trackIx);
		void PlayTrack(string path);
		void playerDone();
		void playNextTrack();

		time_t trackStart;
		Player *player;
		string musicDirectory;
		vector<album> albums;
		unsigned int albumIndex;
		unsigned int trackIndex;
};

#endif
