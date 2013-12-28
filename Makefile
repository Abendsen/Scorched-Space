all: main
CC= g++
CFLAGS= -std=c++0x -Wall -o
LIBS= -lGLEW -lGL -lGLU -lglut
OBJECTS= main.o loadShaders.o draw.o CollisionDetector.o satellite.o

main: ${OBJECTS} 
	$(CC) ${OBJECTS} $(LIBS) $(CFLAGS) main

main.o: main.cpp constants.hpp
	$(CC) -c main.cpp 

loadShaders.o: shaders/loadShaders.c shaders/loadShaders.h
	$(CC) -c shaders/loadShaders.c

draw.o: draw.cpp draw.hpp constants.hpp
	$(CC) -c draw.cpp

CollisionDetector.o: CollisionDetector.cpp CollisionDetector.hpp constants.hpp
	$(CC) -c CollisionDetector.cpp

satellite.o: satellite.cpp satellite.hpp constants.hpp
	$(CC) -c satellite.cpp

clean:
	rm -f main *.o
