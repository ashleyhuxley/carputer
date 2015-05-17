/*
 *  This class plays an MP3 on a separate thread.
 *  We have two booleans - playing and paused. 'playing' is set to false to stop the track, 
 *  'paused' is set to true to pause and false to un-pause.
 */

#include "player.h"

#include <string>
#include <ao/ao.h>
#include <mpg123.h>

void Player::Play(string file)
{
	{
		Glib::Mutex::Lock lock (mutex);
		fileName = file;
		playing = true;
		paused = false;
	}

	thread = Glib::Thread::create(sigc::mem_fun(*this, &Player::play), true);
}

bool Player::Pause()
{
	paused = !paused;
	return paused;
}

void Player::Stop()
{
	{
		Glib::Mutex::Lock lock (mutex);
		playing = false;
	}

	if (thread)
	{
		thread->join();
	}
}

mp3id Player::GetTrackId()
{
	mp3id id;
	id.artist = artist;
	id.title = title;
	return id;
}

void Player::play()
{
	mpg123_handle *mh;
	mpg123_id3v1 *v1;
	mpg123_id3v2 *v2;

	unsigned char *buffer;
	size_t buffer_size;
	size_t done;
	int err;

	int driver;
	ao_device *dev;

	ao_sample_format format;
	int channels, encoding;
	long rate;

	/* initializations */
	driver = ao_default_driver_id();
	mh = mpg123_new(NULL, &err);
	buffer_size = mpg123_outblock(mh);

	buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

	/* open the file and get the decoding format */
	mpg123_open(mh, fileName.c_str());
	mpg123_getformat(mh, &rate, &channels, &encoding);
	mpg123_id3(mh, &v1, &v2);

	artist = v2->artist->p;
	title = v2->title->p;
	trackChanged();

	/* set the output format and open the output device */
	format.bits = mpg123_encsize(encoding) * BITS;
	format.rate = rate;
	format.channels = channels;
	format.byte_format = AO_FMT_NATIVE;
	format.matrix = 0;
	dev = ao_open_live(driver, &format, NULL);

	/* decode and play */
	while (playing && (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK))
	{
		while (paused) { }
		ao_play(dev, (char*)buffer, done);
	}

	/* clean up */
	free(buffer);
	ao_close(dev);
	mpg123_close(mh);
	mpg123_delete(mh);
	
	// Dispatcher to tell the caller that the current track has finished playing.
	// If 'playing' is false, it means the track was stopped part way through, so we don't
	// want to call the track finished callback
	if (playing)
	{
		playDone();
	}
}
