INC_DIRS = -I/usr/include -I/glut-3.7/include -I./  -I/usr/local/include \
	-I/usr/local/fltk-1.1.6/
LIB_DIRS = -L/usr/lib -L/glut-3.7/lib/glut -L/usr/local/glut-3.7/lib/glut -L/usr/X11R6/lib -L/usr/local/lib \
	-L/usr/local/fltk-1.1.6/lib

LDLIBS = -lfltk  -lfltk_gl -lglut -lGL -lGLU -lm -lX11
CFLAGS = -g -ansi -Wall -Wno-deprecated

OBJECTS= main.o glMatrix.o gui.o FFD.o SFDWin.o triple.o physics.o
SHARED_DIR=../shared/

all: SFD

SFD: $(OBJECTS)
	g++ $(OBJECTS) ${LIB_DIRS} ${LDLIBS} -o SFD


main.o: main.cpp gui.hpp
	g++ ${CFLAGS} -c main.cpp  ${INC_DIRS}

gui.o: gui.cpp gui.hpp SFDWin.hpp
	g++ ${CFLAGS} -c gui.cpp  ${INC_DIRS}

FFD.o: $(SHARED_DIR)FFD.cpp $(SHARED_DIR)FFD.hpp $(SHARED_DIR)triple.hpp
	g++ ${CFLAGS} -c $(SHARED_DIR)FFD.cpp  ${INC_DIRS}

SFDWin.o: SFDWin.cpp SFDWin.hpp $(SHARED_DIR)triple.hpp $(SHARED_DIR)glMatrix.hpp
	g++ ${CFLAGS} -c SFDWin.cpp  ${INC_DIRS}

triple.o: $(SHARED_DIR)triple.cpp $(SHARED_DIR)triple.hpp $(SHARED_DIR)quad.hpp
	g++ ${CFLAGS} -c $(SHARED_DIR)triple.cpp  ${INC_DIRS}

physics.o: physics.cpp physics.hpp $(SHARED_DIR)triple.hpp $(SHARED_DIR)FFD.hpp $(SHARED_DIR)interpolation.hpp
	g++ ${CFLAGS} -c physics.cpp  ${INC_DIRS}

glMatrix.o: $(SHARED_DIR)glMatrix.cpp $(SHARED_DIR)glMatrix.hpp $(SHARED_DIR)triple.hpp
	g++ ${CFLAGS} -c $(SHARED_DIR)glMatrix.cpp  ${INC_DIRS}


clean:
	-rm *.o

clobber: clean
	-rm SFD

clobber_all: clobber
	-rm gui.cpp
	-rm gui.hpp

