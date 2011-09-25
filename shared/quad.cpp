#include "quad.hpp"
#include <math.h>
#include <iostream>
#include "glMatrix.hpp"
#include "triple.hpp"


// Initialize const static members
const quad quad::X(1,0,0,0);
const quad quad::Y(0,1,0,0);
const quad quad::Z(0,0,1,0);
const quad quad::W(0,0,0,1);

// member functions

quad::quad(const triple& _triple):
  x(_triple.x),y(_triple.y),z(_triple.z),w(0){}

void quad::normalize(void) throw(std::invalid_argument)
{
  if(this->x || this->y || this->z || this->w)
    {
      float length = sqrt(pow(this->x,2)+pow(this->y,2)+
			  pow(this->z,2)+pow(this->w,2));
      this->x=this->x/length;
      this->y=this->y/length;
      this->z=this->z/length;
      this->w=this->w/length;
    }
  else
    {
      throw std::invalid_argument("Attempt to normalize a zero-quad");
    }
}

float quad::getLength(void) const
{
  return sqrt(pow(this->x,2)+pow(this->y,2)+
	      pow(this->z,2)+pow(this->w,2));
}

const quad quad::operator *(float *matrix) const
{
  quad ret;
  const float tx=this->x,ty=this->y,tz=this->z,tw=this->w;
  
  ret.x = matrix[0] * tx + matrix[4] * ty + matrix[8] * tz + tw * matrix[12];
  ret.y = matrix[1] * tx + matrix[5] * ty + matrix[9] * tz + tw * matrix[13];
  ret.z = matrix[2] * tx + matrix[6] * ty + matrix[10] * tz + tw * matrix[14];
  ret.w = matrix[3] * tx + matrix[7] * ty + matrix[11] * tz + tw * matrix[15];
  return ret;
}
const quad quad::operator *(const glMatrixf &matrix) const
{
  quad ret;
  const float tx=this->x,ty=this->y,tz=this->z,tw=this->w;
  
  ret.x = matrix[0] * tx + matrix[4] * ty + matrix[8] * tz + tw * matrix[12];
  ret.y = matrix[1] * tx + matrix[5] * ty + matrix[9] * tz + tw * matrix[13];
  ret.z = matrix[2] * tx + matrix[6] * ty + matrix[10] * tz + tw * matrix[14];
  ret.w = matrix[3] * tx + matrix[7] * ty + matrix[11] * tz + tw * matrix[15];
  return ret;
}
