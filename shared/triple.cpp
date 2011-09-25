#include "triple.hpp"
#include <math.h>
#include <iostream>
#include "glMatrix.hpp"
#include "quad.hpp"



// Initialize const static members
const triple triple::X(1,0,0);
const triple triple::Y(0,1,0);
const triple triple::Z(0,0,1);

// non member functions

triple cross(triple const &A, triple const &B)
{
  float x, y, z;
  x=A.y*B.z-A.z*B.y;
  y=A.z*B.x-A.x*B.z;
  z=A.x*B.y-A.y*B.x;
  return triple(x,y,z);
}

// member functions

triple::triple(const quad& _quad):
  x(_quad.x),y(_quad.y),z(_quad.z){}

void triple::normalize(void) throw(std::invalid_argument)
{
  if(this->x || this->y || this->z)
    {
      float length = sqrt(pow(this->x,2)+pow(this->y,2)+pow(this->z,2));
      this->x=this->x/length;
      this->y=this->y/length;
      this->z=this->z/length;
    }
  else
    {
      throw std::invalid_argument("Attempt to normalize a zero-triple");
    }
}

float triple::getLength(void) const
{
  return sqrt(pow(this->x,2)+pow(this->y,2)+pow(this->z,2));
}

const triple triple::operator *(float *matrix) const
{
  triple ret;
  const float tx=this->x,ty=this->y,tz=this->z;
  
  ret.x = matrix[0] * tx + matrix[4] * ty + matrix[8] * tz + matrix[12];
  ret.y = matrix[1] * tx + matrix[5] * ty + matrix[9] * tz + matrix[13];
  ret.z = matrix[2] * tx + matrix[6] * ty + matrix[10] * tz + matrix[14];
  return ret;
}
const triple triple::operator *(const glMatrixf &matrix) const
{
  triple ret;
  const float tx=this->x,ty=this->y,tz=this->z;
  
  ret.x = matrix[0] * tx + matrix[4] * ty + matrix[8] * tz + matrix[12];
  ret.y = matrix[1] * tx + matrix[5] * ty + matrix[9] * tz + matrix[13];
  ret.z = matrix[2] * tx + matrix[6] * ty + matrix[10] * tz + matrix[14];
  return ret;
}
