OBJ = main.o musicplayer.o player.o MainWindow.o AlbumWindow.o gpsinfo.o Speedo.o PostcodeWindow.o PostcodeImage.o gpio.o GpioWatcher.o

DEPS = musicplayer.h player.h MainWindow.h AlbumWindow.h rect.h gpsinfo.h Speedo.h PostcodeWindow.h PostcodeImage.h gpio.h GpioWatcher.h

LIBS = -lgps -lao -lmpg123 -lrt `pkg-config --libs gtkmm-3.0` `pkg-config --libs gthread-2.0` -lpthread -ldl -lsqlite3

CFLAGS = -std=c++11 -lrt -lconfig++ `pkg-config --cflags gtkmm-3.0` `pkg-config --cflags gthread-2.0`

CC = g++

EXTENSION = .cpp

%.o: %$(EXTENSION) $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

carputer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

#Cleanup
.PHONY: clean

clean:
	rm -f *.o *~ core *~ 

