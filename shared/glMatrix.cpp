#include "glMatrix.hpp"
glMatrixf::glMatrixf()
{
  for(int i=0;i<16;i++)
    data[i]=0;
}
glMatrixd::glMatrixd()
{
  for(int i=0;i<16;i++)
    data[i]=0;
}
glMatrixf::glMatrixf(const glMatrixf &toCopy)
{
  for(int i=0;i<16;i++)
    data[i]=toCopy.data[i];
}
glMatrixd::glMatrixd(const glMatrixd &toCopy)
{
  for(int i=0;i<16;i++)
    data[i]=toCopy.data[i];
}

void glMatrixf::getGLModelview()
{
  glGetFloatv(GL_MODELVIEW_MATRIX,data);
}

void glMatrixd::getGLModelview()
{
  glGetDoublev(GL_MODELVIEW_MATRIX,data);
}

void glMatrixf::getGLProjection()
{
  glGetFloatv(GL_PROJECTION_MATRIX,data);
}

void glMatrixd::getGLProjection()
{
  glGetDoublev(GL_PROJECTION_MATRIX,data);
}

void glMatrixf::loadIdentity()
{
  for(size_t i=0;i<16;i++)
    (*this)[i]=(i%5)?0:1;
}

void glMatrixd::loadIdentity()
{
  for(size_t i=0;i<16;i++)
    (*this)[i]=(i%5)?0:1;
}
