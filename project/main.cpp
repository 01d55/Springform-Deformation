#include <FL/Fl.H>
#include "gui.hpp"

//This is really all you need to do here!!!
int main(int argc, char **argv){
  Fl::gl_visual(FL_DEPTH | FL_DOUBLE);
  GUI *g = new GUI();
  g->show();
  
  return Fl::run();
}
