CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

project: project.o image_manip.o ppm_io.o
	$(CC) -o project project.o image_manip.o ppm_io.o -lm
project.o: project.c image_manip.h ppm_io.h
	$(CC) $(CFLAGS) -c project.c
image_manip.o: image_manip.c image_manip.h
	$(CC) $(CFLAGS) -c image_manip.c
ppm_io.o: ppm_io.c ppm_io.h
	$(CC) $(CFLAGS) -c ppm_io.c
clean:
	rm -f *.o project