/*
 *  This class represents a simple MP3 player. The actual MP3 playing is done in a different class (player.c)
 *  for some reason. I could merge them but I've done it this way now.
 *  It's a fairly simple player and the files have to be laid out in a certain way in the music directory.
 *  Each directory within the music directory is one album, named in the format "Artist - Album Name".
 *  Any files in the music dir root are ignored.
 *  Only MP3 files within the album directories are loaded. They should be named starting with the track
 *  number so they are loaded in order; "01 Artist - Track name.mp3"
 *  There must also be a 160x160px file called albumart.png in the album directory
 *  Anything else in the album dir is ignored. 
 */

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "musicplayer.h"

inline unsigned int GetRandomNumber(int nLow, int nHigh)
{
    return (rand() % (nHigh - nLow + 1)) + nLow;
}

MusicPlayer::MusicPlayer(string musicDir)
{
	musicDirectory = musicDir;
	player = new Player();
	player->playDone.connect(sigc::mem_fun(*this, &MusicPlayer::playerDone));
	player->trackChanged.connect(sigc::mem_fun(*this, &MusicPlayer::trackChanged));
	ao_initialize();
	mpg123_init();
	time(&trackStart);
}

void MusicPlayer::Load()
{
	loadAlbums();
}

bool MusicPlayer::Pause()
{
	if (player == NULL)
	{
		return false;
	}

	return player->Pause();
}

void MusicPlayer::Skip()
{
	if (player == NULL)
	{
		return;
	}

	player->Stop();
	playNextTrack();
}

void MusicPlayer::Back()
{
	if (player == NULL)
	{
		return;
	}

	player->Stop();
	
	// Go back one track, but only if we're at the start of the song and not at the 1st track.
	// Otherwise, just go back to the beginning of this track.
	if (trackIndex > 0)
	{
		time_t now;
		double seconds;
		time(&now);
		seconds = difftime(now, trackStart);
		
		if (seconds <= 2)
		{
			trackIndex--;
		}
	}
	
	PlayTrack(ResolveTrackPath(albumIndex, trackIndex));
}

void MusicPlayer::PlayRandomAlbum()
{
	if (albums.size() == 0)
	{
		cout << "Nothing to play! Call Load first." << endl;
		return;
	}

	SwitchAlbum(GetRandomNumber(0, albums.size() - 1));
}

type_signal_trackchanged MusicPlayer::signal_trackchanged()
{
	return m_signal_trackchanged;
}

type_signal_albumchanged MusicPlayer::signal_albumchanged()
{
	return m_signal_albumchanged;
}

string MusicPlayer::GetAlbumArt(int albumId)
{
	return musicDirectory + "/" + albums[albumId].name + "/albumart.png";
}

void MusicPlayer::trackChanged()
{
	time(&trackStart);
	
	mp3id id = player->GetTrackId();
	m_signal_trackchanged.emit(id.artist, id.title);
}

void MusicPlayer::loadAlbums()
{
	albums = vector<album>();

	DIR *dir;
	DIR *alDir;

	struct dirent *ent;
	struct dirent *alEnt;

	if ((dir = opendir (musicDirectory.c_str())) != NULL)
	{
	  while ((ent = readdir(dir)) != NULL)
	  {
		if (ent->d_name[0] == '.')
		{
			continue;
		}

		album a;
		a.name = ent->d_name;
		a.tracks = vector<string>();

		string albumDir = musicDirectory;
		albumDir += "/";
		albumDir += a.name;

		if ((alDir = opendir (albumDir.c_str())) != NULL)
		{
		  while ((alEnt = readdir(alDir)) != NULL)
		  {
			if (alEnt->d_name[0] == '.')
			{
				continue;
			}
	
			string name = alEnt->d_name;
			if (name.find(".mp3") != string::npos)
			{
				a.tracks.push_back(name);
			}
		  }
		  
		  closedir (alDir);
		}
		else
		{
			printf("Error loading tracks");
		}

		albums.push_back(a);
	  }

	  closedir (dir);
	}
	else
	{
		printf("Error loading albums");
	}

	sort (albums.begin(), albums.end(), [] (album i , album j) { return (i.name < j.name); });
}

void MusicPlayer::SwitchAlbum(int index)
{
	albumIndex = index;
	trackIndex = 0;

	player->Stop();

	m_signal_albumchanged.emit(albumIndex);

	PlayTrack(ResolveTrackPath(albumIndex, trackIndex));
}

string MusicPlayer::ResolveTrackPath(int albumIx, int trackIx)
{
	return musicDirectory + "/" + albums[albumIx].name + "/" + albums[albumIx].tracks[trackIx];
}

void MusicPlayer::PlayTrack(string path)
{
	player->Play(path);
}

void MusicPlayer::playNextTrack()
{
	trackIndex++;

	if (trackIndex >= albums[albumIndex].tracks.size())
	{
		PlayRandomAlbum();
		return;
	}

	PlayTrack(ResolveTrackPath(albumIndex, trackIndex));
}

// Callback from player.cpp - triggered at the end of a track
void MusicPlayer::playerDone()
{
	playNextTrack();
}

unsigned int MusicPlayer::GetCurrentAlbumIndex()
{
	return albumIndex;
}

string MusicPlayer::GetAlbumName(unsigned int idx)
{
	return albums[idx].name;
}

unsigned int MusicPlayer::GetAlbumCount()
{
	return albums.size();
}
