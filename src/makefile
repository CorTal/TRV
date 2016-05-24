#authors: Corentin Talarmain, Nicolas Roux, Théo Voillemin
CPP = *.cpp

FLAGS = -O3 -funroll-loops --param max-unroll-times=8 -I/usr/include/qt4 -l QtCore -l QtGui -pthread -ftree-vectorize -msse4.2 -Wall -std=c++11 $(shell pkg-config --cflags --libs libxml++-2.6)

all: 
	moc displayqt.h -o moc_displayqt.cpp
	moc controller.h -o moc_controller.cpp 
	g++ $(CPP) $(FLAGS) -o trv.exe

clean:
	@echo "-> Clean project"
	rm -rf *.exe
