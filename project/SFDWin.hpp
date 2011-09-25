#ifndef SFDWin_guard
#define SFDWin_guard

#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "../shared/triple.hpp"
#include "../shared/FFD.hpp"
#include "physics.hpp"


class SFDWin: public Fl_Gl_Window
{
public:

  SFDWin( int,int,int,int,const char *l = 0 );
  virtual ~SFDWin() throw();

  void readFile(char * fname);
  void readFile(std::string fname) throw(std::invalid_argument);
  void readFile(std::ifstream &coor,std::ifstream &poly,std::ifstream &grid)
    throw(std::invalid_argument);

  void rotatex(double);
  void rotatey(double);
  void rotatez(double);
  void zoom(double);

  // For handling FLTK callbacks. Pointed-to values will be cast to bool.
  void displayGrid(char);
  void displayModel(char);

  void startAnimation();
  void resetAnimation();
  void reset();
protected:
private:
  // private members
  GLubyte pickname;
  float twist;
  FFD *deform;
  physics *springs;
  std::vector< std::vector<size_t> > polyPointIndices;
  std::vector<triple> points, normals;
  const std::vector<triple> *tpoints, *tnormals;
  triple rotation;
  float scale;
  bool d_model, d_grid;

  // private functions
  void draw();
  int handle(int);
  void setupGL();
  void pick(int,int,int);
  void drag(int,int,int);
  void release(int,int,int);

  void readPoly(std::ifstream &);
  void readCoor(std::ifstream &);
  void readGrid(std::ifstream &);
  void defaultGrid();
  void generateNormals();

  /* FLTK timeout handlers are somewhat arcane.
   */
  static void Fl_timeout_handle(void *);
  static void end_fl_timeout(void *);


};

inline void SFDWin::rotatex(double x)
{ rotation.x=x; }
inline void SFDWin::rotatey(double y)
{ rotation.y=y; }
inline void SFDWin::rotatez(double z)
{ rotation.z=z; }
inline void SFDWin::zoom(double z)
{ scale= -(1.0/z); }
inline void SFDWin::displayGrid(char b)
{ d_grid=(bool)b; }
inline void SFDWin::displayModel(char b)
{ d_model=(bool)b; }

#endif // ifndef SFDWin_guard
