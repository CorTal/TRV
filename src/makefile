#authors: Corentin Talarmain, Nicolas Roux, Théo Voillemin
CPP = *.cpp

FLAGS = -O3 -ggdb -Wall -std=c++11 $(shell pkg-config --cflags --libs libxml++-2.6)

all: 
	moc displayqt.h -o moc_displayqt.cpp
	moc controller.h -o moc_controller.cpp 
	g++ $(CPP) $(FLAGS) -I/usr/include/qt4 -l QtCore -l QtGui -pthread -o trv.exe

clean:
	@echo "-> Clean project"
	rm -rf *.exe
