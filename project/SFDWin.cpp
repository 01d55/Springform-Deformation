#include "SFDWin.hpp"
#include "../shared/interpolation.hpp"
#include "../shared/glMatrix.hpp"
#include "../shared/triple.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <iterator>

// 30 FPS for 10 sec
static const float delay = 1.0/30, runtime=30.0;

// Default values: set when constructed and when reset is called.
static const triple eye(5,5,5),center;

/* Values used for OpenGL */
static const GLfloat ORTHO=60.0, CLIPSIZE=512.0, FOV=60.0;
static const float PI =3.14159, FULL_TWIST =PI/2, TWIST_STEP=FULL_TWIST/30;
static const GLsizei bufsize=32;
static GLuint selbuf[bufsize];
static GLint vp[4];



// Public members

SFDWin::SFDWin( int x,int y,int w,int h,const char *l):
  Fl_Gl_Window(x,y,w,h,l),pickname(0),twist(0.0125),deform(NULL),springs(NULL),
  polyPointIndices(),points(),normals(),rotation(),scale(0.03f),
  d_model(true),d_grid(true)
{
}


SFDWin::~SFDWin() throw ()
{
  if(springs)
    delete springs;
  if(deform)
    delete deform;
}

void SFDWin::readFile(char * fname)
{
  if(fname)
    { // GUI might pass us NULL
      std::string toString(fname);
      readFile(toString);
    }
}

void SFDWin::readFile(std::string fname) throw(std::invalid_argument)
{
  std::ifstream coor,poly,grid;
  char ext[6]; // expected value: {.,c,o,o,r,\0}
  std::size_t length;
  
  length=fname.copy(ext,5,fname.size()-5);
  ext[length]='\0';
  
  if(std::string(".coor") != ext)
    throw std::invalid_argument("SFDWin::readfile(std::string): Attempt to load a file which is not a .coor file.");
  
  coor.open(fname.c_str());
  fname.replace(fname.size()-5,5,".poly");
  poly.open(fname.c_str());
  fname.replace(fname.size()-5,5,".grid");
  readFile(coor,poly,grid);
}

void SFDWin::readFile(std::ifstream &coor,std::ifstream &poly,std::ifstream &grid)
  throw(std::invalid_argument)
{
  if(!(coor.is_open() && poly.is_open()))
    throw std::invalid_argument("SFDWin::readfile(std::ifstream &, std::ifstream&, std::ifstream&): failed to open coor or failed to open poly");

  readCoor(coor);
  readPoly(poly);
  if(grid.is_open())
    readGrid(grid);
  else
    defaultGrid();
  
  generateNormals();
}




void SFDWin::reset()
{
  end_fl_timeout((void*)this); // Stop in-progress animation
  // Don't auto-stop a later animation
  Fl::remove_timeout(end_fl_timeout,(void*)this);

  if(springs)
    delete springs;
  if(deform)
    delete deform;

  springs = NULL;
  deform = NULL;

  this->redraw();
}

void SFDWin::resetAnimation()
{
  end_fl_timeout((void*)this); // Stop in-progress animation
  // Don't auto-stop a later animation
  Fl::remove_timeout(end_fl_timeout,(void*)this);

  if(springs)
    springs->simreset();

  this->redraw();
}

void SFDWin::startAnimation()
{
  Fl::add_timeout(delay,Fl_timeout_handle,(void*)this);
  Fl::add_timeout(runtime,end_fl_timeout,(void*)this);
}

int SFDWin::handle(int event)
{
  static int num,x,y;
  switch( event ){
  case FL_PUSH:
    
    num = Fl::event_button( );
    x = Fl::event_x( );
    y = vp[3] - Fl::event_y( ); // GL puts 0 at the bottom, FL at the top
    pick(num,x,y);
    return 1;
  case FL_DRAG:
    //... mouse moved while down event ...
    num = Fl::event_button( );
    x = Fl::event_x( );
    y = vp[3] - Fl::event_y( ); // GL puts 0 at the bottom, FL at the top
    drag(num,x,y);
    return 1;
  case FL_RELEASE:
    //... mouse up event ...
    num = Fl::event_button( );
    x = Fl::event_x( );
    y = vp[3] - Fl::event_y( ); // GL puts 0 at the bottom, FL at the top
    drag(num,x,y);
    //pickname=NONE;
    return 1;
  case FL_FOCUS :
    return 1;
  case FL_UNFOCUS :
    //... Return 1 if you want keyboard events, 0 otherwise
    return 1;
  case FL_KEYBOARD:
    //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
    //... Return 1 if you understand/use the keyboard event, 0 otherwise...
    return 0;
  case FL_SHORTCUT:
    //... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
    //... Return 1 if you understand/use the shortcut event, 0 otherwise...
    return 0;
  default:
    // tell FLTK that I don't understand other events
    return 0;
  }
}

void SFDWin::draw()
{
  if(!valid())
    setupGL();

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glScalef(scale,scale,scale);
  if(deform)
    {
      glRotatet(rotation.x,triple::X);
      glRotatet(rotation.y,triple::Y);
      glRotatet(rotation.z,triple::Z);
      std::vector< std::vector<size_t> >::const_iterator itor;
      std::vector<size_t>::const_iterator jtor;
      std::vector<triple>::const_iterator ktor;
      if(d_model)
	{
	  tpoints=&deform->getOutput();
	  for(itor=polyPointIndices.begin();itor!=polyPointIndices.end();itor++)
	    {
	      jtor=itor->begin();
	      glBegin(GL_POLYGON);
	      for(; jtor!=itor->end(); jtor++)
		{
		  
		  glNormalt(normals[*jtor]);
		  glVertext((*tpoints)[*jtor]);
		  //glVertext(points[*jtor]);
		  
		}
	      glEnd();
	      
	    }
	}
      if(d_grid)
	{
	  tpoints=&deform->getControlPoints();
	  glDisable(GL_LIGHTING);
	  glColort(triple::Y);
	  for(size_t i=0;i<4;i++)
	    for(size_t j=0;j<4;j++)
	      {
		glBegin(GL_LINE_LOOP);
		for(size_t k=0;k<4;k++)
		  {
		    glVertext((*tpoints)[4*4*i+4*j+k]);
		  }
		glEnd();
		glBegin(GL_LINE_LOOP);
		for(size_t k=0;k<4;k++)
		  {
		    glVertext((*tpoints)[i+4*j+4*4*k]);
		  }
		glEnd();
	      }
	  glEnable(GL_LIGHTING);
	}
    }
  glPopMatrix();
}

// private members

/* Pick only one pickable thing. Priority is determined by the order
   in which pickable things are drawn.
 */
void SFDWin::pick(int num, int x, int y)
{
  int hits;
  GLdouble dx=20,dy=20;

  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(10);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  {
    glLoadIdentity();
    gluPickMatrix(x,y,dx,dy,vp);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
    }
    glPopMatrix();
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  if( (hits = glRenderMode(GL_RENDER))) // If hits != 0
    {
      /* OpenGL stores all information regarding a given hit in a single
	 GLuint. Each byte of the GLuint corresponds to different information.
	 There are 4 GLubytes in each GLuint.
      */ 
      /*
      switch(selbuf[3]) // Name of the first hit
	{
	case POINT1:
	  pickname = POINT1;
	  std::cout << "P1\n";
	  break;
	case POINT2:
	  pickname = POINT2;
	  std::cout << "P2\n";
	  break;
	case POINT3:
	  pickname = POINT3;
	  std::cout << "P3\n";
	  break;

	case BALL_EDGE:
	  pickname = BALL_EDGE;
	  std::cout << "EDGE\n";
	  break;
	case BALL_CENTER:
	  pickname = BALL_CENTER;
	  std::cout << "CENTER\n";
	  break;
	case NONE:
	  pickname = NONE;
	  std::cout << "NONE\n";
	  break;
	default:
	  pickname = NONE;
	  std::cout << "DEFAULT\n";
	  break;
	}
      */
    }
  std::cout << std::flush;
  
}


void SFDWin::drag(int num, int _x, int _y)
{
  triple *pointer=NULL;
  glMatrixd model, project;
  GLdouble objX,objY,objZ;
  triple win((float)_x,(float)_y,0),obj;

  model.getGLModelview();
  project.getGLProjection();
  if(GLU_FALSE == gluUnProject(win,model,project,vp,&objX,&objY,&objZ))
    throw std::logic_error("SFDwin::drag(int,int,int): gluUnProject failed");
  obj(objX,objY,0);
  /*
  switch(pickname)
    {
    case POINT1:
      pointer = &p1;
      break;
    case POINT2:
      pointer = &p2;
      break;
    case POINT3:
      pointer = &p3;
      break;
    case BALL_CENTER:
      {
	pointer= &ball;
	if(sim.getStateAt(objX)==SLOPE2)
	{ 
	  float T=(objX-p2.x)/(p3.x-p2.x);
	  objY=lerp(p2.y,p3.y,T);
	}
	else
	  {
	    float T=(objX-p1.x)/(p2.x-p1.x);
	    objY=lerp(p1.y,p2.y,T);
	  }
	ball_start.x=objX;
	ball_start.y=objY;
	break;
      }
    case BALL_EDGE:
      radius = (obj-ball).getLength();
      this->redraw();
      return;
    default:
      return;
    }
  pointer->x=objX;
  pointer->y=objY;
  */
  this->redraw();
}

void SFDWin::setupGL()
{
  int w=this->w(),h=this->h();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV,(float)w/(float)h,1,CLIPSIZE);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye,center,triple::Y);
  
  glViewport(0,0,w,h);
  glGetIntegerv(GL_VIEWPORT,vp);

  glSelectBuffer(bufsize,selbuf);

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat pos[] = {0,1,0,0};
  GLfloat dir[] = {0,-1,0};
  GLfloat lightYellow[] = {0.5,0.5,0,1};
  glLightfv(GL_LIGHT0, GL_POSITION,pos);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,dir);
  glLightfv(GL_LIGHT0, GL_SPECULAR,lightYellow);
  glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE,lightYellow);
  /*
  glLightfv(GL_LIGHT1, GL_POSITION,pos);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,dir);
  glLightfv(GL_LIGHT1, GL_SPECULAR,lightYellow);
  glLightfv(GL_LIGHT1, GL_AMBIENT,lightYellow);
  */

  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,4);
  
}
 
void SFDWin::readPoly(std::ifstream &poly)
{
  size_t polynum;
  poly >> polynum;
  polyPointIndices.resize(polynum);
  // Get out of the first line
  poly.seekg(1,std::ios::cur);
  for(size_t i=0;i<polynum;i++){
	// The first word is a useless name that we don't use.
	std::string line;
	getline(poly,line);
	std::string::iterator it=line.begin();
	while((*it!=' ') && (it!=line.end())){
	  *it=' ';
	  it++;
	}
	std::stringstream readable(line);
	// Read all the ints on the line
	int value;
	while(readable.good()){
	  readable >> std::skipws >> value;
	  polyPointIndices[i].push_back(value-1);//.poly files index from 1, arrays index from 0.
	}
  }
  
}
void SFDWin::readCoor(std::ifstream &coor)
{
  size_t coornum;
  coor >> coornum;
  points.resize(coornum);
  normals.resize(coornum);
  // Get past the first line
  coor.seekg(1,std::ios::cur);
  for(size_t i=0;i<coornum;i++){
	// ostream >> float doesn't ignore commas, so we gotta kill 'em.
	std::string line;
	std::string::iterator it;
	getline(coor,line);
	for(it = line.begin();it!=line.end();it++)
		if(*it ==',')
		  *it=' ';
	std::stringstream readable(line);
	//
	int junk; // Each triple is preceded by an extraneous line number.
	readable >> junk;
	readable >> points[i].x;
	readable >> points[i].y;
	readable >> points[i].z;
  }
  coor.close();
}
void SFDWin::readGrid(std::ifstream &grid)
{
  if(springs)
    delete springs;
  if(deform)
    delete deform;
  deform = new FFD(points);
  springs = new physics((*deform));
}
void SFDWin::defaultGrid()
{
  if(springs)
    delete springs;
  if(deform)
    delete deform;
  deform = new FFD(points);
  springs = new physics((*deform));
}

void SFDWin::generateNormals()
{
  std::vector< std::vector<size_t> >::iterator itor;
  std::vector<size_t>::iterator jtor;
  std::vector<triple>::iterator ktor;

  for(itor=polyPointIndices.begin();itor!=polyPointIndices.end();
      itor++)
    {
      triple polyNorm,A,B;
      jtor=itor->begin();
      if(jtor==itor->end())
	continue;
      for(;(jtor+1)!=itor->end();jtor++)
	{
	  A=points[*(jtor)]-points[*(itor->begin())];
	  B=points[*(jtor+1)]-points[*(itor->begin())];
	  polyNorm = cross(A,B);
	  if(polyNorm.getLength())
	    {
	      polyNorm.normalize();
	      break;
	    }
	}
      for(jtor=itor->begin(); jtor!=itor->end(); jtor++)
	{
	  normals[*jtor] = normals[*jtor] + polyNorm;
	}
    }
  size_t temp=0;
  for(ktor=normals.begin(); ktor!=normals.end(); ktor++)
    {
      ++temp;
      ktor->normalize();
    }
}

/* FLTK timeout handler - ugh void* casting
   The function is declared static because pointers to non-static member
   functions are not equivalent to pointers to a normal function.
 */
void SFDWin::Fl_timeout_handle(void *aSFDWin)
{
  SFDWin *ptr=(SFDWin*)aSFDWin;

  /*
  glMatrixf transform;
  float A,B,tcos,tsin;

  const std::vector<triple> &cPoints=ptr->deform->getControlPoints();
  std::vector<triple>::const_iterator itor;
  size_t i,j;

  A=ptr->deform->getOrigin().x;
  B=ptr->deform->getOrigin().x+ptr->deform->getS().x;

  transform.loadIdentity();
  
  for(itor=cPoints.begin();itor!=cPoints.end();++itor)
    {
      tcos=cos( TWIST_STEP*rlerp(A,B,(*itor).x) );
      tsin=sin( TWIST_STEP*rlerp(A,B,(*itor).x) );
      transform[5]=transform[10]=tcos;
      transform[6]=tsin;
      transform[9]=-tsin;
      i=distance(cPoints.begin(),itor);
      triple T = (*itor)*transform;
      ptr->deform->setPoint(i,T);
    }
  */
  ptr->springs->simstep();

  ptr->redraw();
  Fl::repeat_timeout(delay,Fl_timeout_handle,aSFDWin);
}

void SFDWin::end_fl_timeout(void* aSFDWin)
{
  SFDWin *ptr=(SFDWin*)aSFDWin;
  Fl::remove_timeout(Fl_timeout_handle,aSFDWin);
  std::cout << "Animation halting" << std::endl;
  ptr->redraw();
}

// Helper functions
