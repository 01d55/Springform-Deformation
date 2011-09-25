#ifndef triple_guard
#define triple_guard
#include <FL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <stdexcept>

class glMatrixf;
class quad;
class triple
{
public:
  triple():
    x(0),y(0),z(0) {}
  triple(float _x,float _y,float _z):
    x(_x),y(_y),z(_z) {}
  triple (const triple &_triple):
    x(_triple.x),y(_triple.y),z(_triple.z){}
  triple (const quad &_quad);

  // Operator overloads
  const triple operator +(const triple &right) const;
  const triple operator -(const triple &right) const;
  
  const triple operator ()(float x, float y, float z);
  
  const triple operator()(const triple& tocopy);
  
  const triple operator =(const triple& tocopy);

  friend std::istream& operator >>(std::istream &, triple &);
  friend std::ostream& operator <<(std::ostream &, triple &);
  
  float& operator[](size_t i) throw(std::out_of_range);
  const float& operator[](size_t i) const throw(std::out_of_range);
  const triple operator *(float factor) const;
  // matrix absolutely must be an openGL matrix (size 16 array).
  const triple operator *(float *matrix) const;
  const triple operator *(const glMatrixf &matrix) const;
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
    };
    float xyz[3];
  };
  static const triple X,Y,Z;
  private:
};

inline void glColort(const triple &color); // Alias for glcolor3f to use the triple struct
inline void glNormalt(const triple &norm);
inline void glVertext(const triple &xyz); // Alias for glVertex3f
inline void glTranslatet(const triple &xyz); // Alias for glTranslatef
inline void glRotatet(const float angle, const triple &xyz); // Alias for glRotate
inline void glScalet(const triple &xyz); // Alias for glScalef
inline void gluLookAt(const triple &eye, const triple &center, const triple &up); // Alias for gluLookAt

triple cross(triple const &A, triple const &B);

// Time for definitions

// Inline members

inline float& triple::operator[](size_t i) throw(std::out_of_range)
{
  if(i>2)
    {
      std::cerr << "WARNING: triple::operator[] called out of range: " << i << std::endl;
      throw std::out_of_range("triple::operator[]");
    }
  return xyz[i];
}
inline const float& triple::operator[](size_t i) const throw(std::out_of_range)
{
  switch(i)
    {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    default:
      std::cerr << "WARNING: triple::operator[] called out of range" << i << std::endl;
      throw std::out_of_range("triple::operator[] const");
    }
}

inline const triple triple::operator +(const triple &right) const
{
  triple result(x+right.x,y+right.y,z+right.z);
  return result;
}

inline const triple triple::operator -(const triple &right) const
{
  triple result(x-right.x,y-right.y,z-right.z);
  return result;
}

inline const triple triple::operator *(float factor) const
{
  return triple(x*factor,y*factor,z*factor);
}

inline const triple triple::operator ()(float x, float y, float z)
{
  this->x=x;
  this->y=y;
    this->z=z;
    return *this;
}


inline const triple triple::operator()(const triple& tocopy)
{
  return (*this)(tocopy.x,tocopy.y,tocopy.z);
}
inline const triple triple::operator =(const triple& tocopy)
{
  return (*this)(tocopy);
}
 
// Inline friends
inline std::istream& operator >>(std::istream &is, triple &trip)
{
  is>>trip.x>>trip.y>>trip.z;
  return is;
}

inline std::ostream& operator <<(std::ostream &os, triple &trip)
{
  return os << trip.x << " " 
	    << trip.y << " "
	    << trip.z;
}
// Non member inline
inline void glTranslatet( const triple &xyz)
{
  glTranslatef(xyz.x,xyz.y,xyz.z);
}

inline void glRotatet(float angle, const triple &xyz)
{
  glRotatef(angle, xyz.x, xyz.y, xyz.z);
}

inline void glColort(const triple &color)
{
  glColor3f(color.x,color.y,color.z);
}

inline void glVertext(const triple &xyz)
{
  glVertex3f(xyz.x,xyz.y,xyz.z);
}

inline void gluLookAt(const triple &eye, const triple &center, const triple &up)
{
  gluLookAt(eye.x,eye.y,eye.z,
			center.x,center.y,center.z,
			up.x,up.y,up.z);
}

inline void glNormalt(const triple &norm)
{
  glNormal3f(norm.x,norm.y,norm.z);
}

inline void glScalet(const triple &xyz)
{
  glScalef(xyz.x,xyz.y,xyz.z);
}

#endif //ifndef triple_guard
