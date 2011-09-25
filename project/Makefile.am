bin_PROGRAMS= SFD
SFD_SOURCES = gui.hpp physics.hpp SFDWin.hpp\
gui.cpp physics.cpp SFDWin.cpp main.cpp\
../shared/FFD.hpp ../shared/glMatrix.hpp ../shared/interpolation.hpp ../shared/quad.hpp ../triple.hpp\
../shared/FFD.cpp ../shared/glMatrix.cpp ../shared/quad.cpp ../shared/triple.cpp
SFD_LDFLAGS = $(LDFLAGS) `fltk-config --use-gl --ldflags` -lGLU
