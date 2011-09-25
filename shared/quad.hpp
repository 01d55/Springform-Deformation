#ifndef quad_guard
#define quad_guard
#include <FL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <stdexcept>

class glMatrixf;
class triple;
class quad
{
public:
  quad():
    x(0),y(0),z(0),w(0) {}
  quad(float _x,float _y,float _z,float _w):
    x(_x),y(_y),z(_z),w(_w) {}
  quad (const quad &_quad):
    x(_quad.x),y(_quad.y),z(_quad.z),w(_quad.w){}
  quad (const triple &_triple);


  // Operator overloads
  const quad operator +(const quad &right) const;
  const quad operator -(const quad &right) const;
  
  const quad operator ()(float x, float y, float z, float w);
  
  const quad operator()(const quad& tocopy);
  
  const quad operator =(const quad& tocopy);

  friend std::istream& operator >>(std::istream &, quad &);
  friend std::ostream& operator <<(std::ostream &, quad &);
  
  float& operator[](size_t i) throw(std::out_of_range);
  const float& operator[](size_t i) const throw(std::out_of_range);
  const quad operator *(float factor) const;
  // matrix absolutely must be an openGL matrix (size 16 array).
  const quad operator *(float *matrix) const;
  const quad operator *(const glMatrixf &matrix) const;
  // Make a normal
  void normalize(void) throw(std::invalid_argument);
  // Find the length
  float getLength(void) const;

  union
  {
    struct
    {
      float x;
      float y;
      float z;
      float w;
    };
    float xyzw[4];
  };
  static const quad X,Y,Z,W;
  private:
};

inline void glColorq(const quad &color); // Alias for glcolor3f to use the quad struct
inline void glNormalq(const quad &norm);
inline void glVertexq(const quad &xyzw); // Alias for glVertex3f
inline void glTranslateq(const quad &xyzw); // Alias for glTranslatef
inline void glRotateq(const float angle, const quad &xyz); // Alias for glRotate
inline void glScaleq(const quad &xyzw); // Alias for glScalef

// Time for definitions

// Inline members

inline float& quad::operator[](size_t i) throw(std::out_of_range)
{
  if(i>3)
    {
      std::cerr << "WARNING: quad::operator[] called out of range: " << i << std::endl;
      throw std::out_of_range("quad::operator[]");
    }
  return xyzw[i];
}
inline const float& quad::operator[](size_t i) const throw(std::out_of_range)
{
  if(i>3)
    {
      std::cerr << "WARNING: quad::operator[] called out of range: " << i << std::endl;
      throw std::out_of_range("quad::operator[]");
    }
  return xyzw[i];
}

inline const quad quad::operator +(const quad &right) const
{
  quad result(x+right.x,y+right.y,z+right.z,w+right.w);
  return result;
}

inline const quad quad::operator -(const quad &right) const
{
  quad result(x-right.x,y-right.y,z-right.z,w-right.w);
  return result;
}

inline const quad quad::operator *(float factor) const
{
  return quad(x*factor,y*factor,z*factor,w*factor);
}

inline const quad quad::operator ()(float x, float y, float z, float w)
{
  this->x=x;
  this->y=y;
  this->z=z;
  this->w=w;
  return *this;
}


inline const quad quad::operator()(const quad& tocopy)
{
  return (*this)(tocopy.x,tocopy.y,tocopy.z,tocopy.w);
}
inline const quad quad::operator =(const quad& tocopy)
{
  return (*this)(tocopy);
}
 
// Inline friends
inline std::istream& operator >>(std::istream &is, quad &trip)
{
  is>>trip.x>>trip.y>>trip.z>>trip.w;
  return is;
}

inline std::ostream& operator <<(std::ostream &os, quad &trip)
{
  return os << trip.x << " " 
	    << trip.y << " "
	    << trip.z << " "
	    << trip.w;
}
// Non member inline
inline void glTranslatet( const quad &xyz)
{
  glTranslatef(xyz.x,xyz.y,xyz.z);
}

inline void glRotatet(float angle, const quad &xyz)
{
  glRotatef(angle, xyz.x, xyz.y, xyz.z);
}

inline void glColorq(const quad &color)
{
  glColor4fv(color.xyzw);
}

inline void glVertexq(const quad &xyzw)
{
  glVertex4fv(xyzw.xyzw);
}

inline void glNormalq(const quad &norm)
{
  glNormal3fv(norm.xyzw);
}

inline void glScaleq(const quad &xyz)
{
  glScalef(xyz.x,xyz.y,xyz.z);
}

#endif //ifndef quad_guard
