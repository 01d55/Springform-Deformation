#ifndef glMatrix_hpp_guard
#define glMatrix_hpp_guard
#include <iostream>
#include <stdexcept>
#include <vector>
#include <FL/gl.h>
#include "triple.hpp"


;
class glMatrixd;

class glMatrixf
{
public:
  friend class glMatrixd;
  glMatrixf();
  glMatrixf(const glMatrixf &);
  glMatrixf(const glMatrixd &);

  void glMultMatrix() const;
  void getGLModelview();
  void getGLProjection();
  void loadIdentity();

  const glMatrixf& operator=(const glMatrixf &);
  const glMatrixf& operator=(const glMatrixd &);
  float& operator[](size_t i);
  const float& operator[](size_t i) const;
  friend std::ostream& operator << (std::ostream &,const glMatrixf &);
private:
  GLfloat data[16];
};

class glMatrixd
{
public:
  friend class glMatrixf;
  // Aliases for gluProject & gluUnProject. The whole reason we even have a
  // glMatrixd
  friend GLint gluProject(const triple &,const glMatrixd&,const glMatrixd&,
			  const GLint*,
			  GLdouble*,GLdouble*,GLdouble*);
  friend GLint gluUnProject(const triple &,const glMatrixd&,const glMatrixd&,
			    const GLint *,
			    GLdouble*,GLdouble*,GLdouble*);

  glMatrixd();
  glMatrixd(const glMatrixd &);
  glMatrixd(const glMatrixf &);

  void glMultMatrix() const;
  void getGLModelview();
  void getGLProjection();
  void loadIdentity();

  const glMatrixd& operator=(const glMatrixd &);
  const glMatrixd& operator=(const glMatrixf &);
  double& operator[](size_t i);
  const double& operator[](size_t i) const;
  friend std::ostream& operator << (std::ostream &,const glMatrixd &);
private:
  GLdouble data[16];
};


// Definitions of inline members

inline void glMatrixd::glMultMatrix() const
{
  glMultMatrixd(data);
}
inline void glMatrixf::glMultMatrix() const
{
  glMultMatrixf(data);
}

inline const glMatrixf& glMatrixf::operator=(const glMatrixf &toCopy)
{
  for(int i=0;i<16;i++)
    {
    data[i]=toCopy.data[i]; 
    }
  return (*this);
}

inline const glMatrixd& glMatrixd::operator=(const glMatrixd &toCopy)
{
  for(int i=0;i<16;i++)
    {
    data[i]=toCopy.data[i]; 
    }
  return (*this);
}

inline const glMatrixf& glMatrixf::operator=(const glMatrixd &toCopy)
{
  for(int i=0;i<16;i++)
    {
      data[i]=(float)toCopy.data[i]; 
    }
  return (*this);
}

inline const glMatrixd& glMatrixd::operator=(const glMatrixf &toCopy)
{
  for(int i=0;i<16;i++)
    {
      data[i]=(double)toCopy.data[i]; 
    }
  return (*this);
}

inline float& glMatrixf::operator[](size_t i)
{
  if(i<0)
    {
      std::cerr << "WARNING: glMatrix indexed less than zero:" << i << std::endl;
      throw std::out_of_range("glMatrix::operator[]");
    }
  if(i>15)
    {
      std::cerr << "WARNING: glMatrix indexed greater than 15:" << i << std::endl;
      throw std::out_of_range("glMatrix::operator[]");
    }
  return data[i];
}

inline double& glMatrixd::operator[](size_t i)
{
  if(i<0)
    {
      std::cerr << "WARNING: glMatrix indexed less than zero:" << i << std::endl;
      throw std::out_of_range("glMatrix::operator[]");
    }
  if(i>15)
    {
      std::cerr << "WARNING: glMatrix indexed greater than 15:" << i << std::endl;
      throw std::out_of_range("glMatrix::operator[]");
    }
  return data[i];
}

inline const float& glMatrixf::operator[](size_t i) const
{  
  if(i<0)
    {
      std::cerr << "WARNING: glMatrix indexed less than zero:" << i << std::endl;
      return data[0];
    }
  if(i>15)
    {
      std::cerr << "WARNING: glMatrix indexed greater than 15:" << i << std::endl;
      return data[16];
    }
  return data[i];
}
inline const double& glMatrixd::operator[](size_t i) const
{  
  if(i<0)
    {
      std::cerr << "WARNING: glMatrix indexed less than zero:" << i << std::endl;
      return data[0];
    }
  if(i>15)
    {
      std::cerr << "WARNING: glMatrix indexed greater than 15:" << i << std::endl;
      return data[16];
    }
  return data[i];
}

// Inline friend definitions

inline GLint gluProject(const triple& obj,
			const glMatrixd& model,const glMatrixd& proj,
			const GLint* view,
			GLdouble* winX,GLdouble* winY,GLdouble* winZ)
{
  return gluProject((double)obj.x,(double)obj.y,(double)obj.z,
		      model.data, proj.data, view,
		      winX, winY, winZ);
}
inline GLint gluUnProject(const triple& win,
			  const glMatrixd& model,const glMatrixd& proj,
			  const GLint* view,
			  GLdouble* objX,GLdouble* objY,GLdouble* objZ)
{
  return gluUnProject((double)win.x,(double)win.y,(double)win.z,
		      model.data, proj.data, view,
		      objX, objY, objZ);
}

inline std::ostream& operator << (std::ostream &os,const glMatrixf &matrix)
{
  for(int i=0;i<4;i++)
    {
      for(int j=0;j<4;j++)
	{
	os<<matrix.data[i+4*j] << " ";
	}
      os<<"\n";
    }
  return os<<std::flush;
}

inline std::ostream& operator << (std::ostream &os,const glMatrixd &matrix)
{
  for(int i=0;i<4;i++)
    {
      for(int j=0;j<4;j++)
	{
	  os<<matrix.data[i+4*j] << " ";
	}
      os<<"\n";
    }
  return os<<std::flush;
}
#endif //ifndef glMatrix_hpp_guard
