CC = g++

all:
	$(CC) matchacids.cpp -o matchacids

clean:
	rm *~ out.txt matchacids

