#include "FFD.hpp"
#include "interpolation.hpp"
#include "glMatrix.hpp"
#include <limits>
/* Forward declarations */

//glMatrixf bezier();
float rlerp(float, float, float);

/* Static consts */

static const size_t GRID_DIM=4,GRID_SIZE = 64;
//static const glMatrixf FFD::bezierM(bezier());

/* Public members */
FFD::FFD(const std::vector<triple> &_geometry):
  geometry(_geometry),
  O(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max()),
  S(std::numeric_limits<float>::min(),0,0),
  T(0,std::numeric_limits<float>::min(),0),
  U(0,0,std::numeric_limits<float>::min()),
  controlPoints(),output(),weights(),output_iscached(false)
{
  unsigned i,j,k;
  float s,t,u;
  std::vector<triple>::const_iterator itor;
  std::vector< std::vector<float> >::iterator jtor;
  // Set up local co-ordinate system. Use the smallest basis which is
  // orthogonal, parallel to standard basis, and encompasses geometry.
  for(itor=geometry.begin();itor!=geometry.end();++itor)
    {
      O.x=std::min<float>(itor->x,O.x);
      O.y=std::min<float>(itor->y,O.y);
      O.z=std::min<float>(itor->z,O.z);
      
      S.x=std::max<float>(itor->x,S.x);
      T.y=std::max<float>(itor->y,T.y);
      U.z=std::max<float>(itor->z,U.z);
    }
  S.x-=O.x;
  T.y-=O.y;
  U.z-=O.z;

  s=S.x/(GRID_DIM-1);
  t=T.y/(GRID_DIM-1);
  u=U.z/(GRID_DIM-1);
  // Resize vectors
  controlPoints.resize(GRID_SIZE);
  weights.resize(geometry.size());
  output.resize(geometry.size());

  for(jtor=weights.begin();jtor!=weights.end();++jtor)
    {
      jtor->resize(GRID_SIZE);
    }
  // Initialize control points.
  for(i=0;i<GRID_DIM;i++)
    for(j=0;j<GRID_DIM;j++)
      for(k=0;k<GRID_DIM;k++)
	controlPoints[GRID_DIM*GRID_DIM*i+GRID_DIM*j+k]=triple(O.x+s*i,O.y+t*j,O.z+u*k);
  // Create weights.
  generateWeights();
}

FFD::~FFD() throw()
{
}

const std::vector<triple> &FFD::getOutput() const
{
  if(!output_iscached)
    generateOutput();
  return output;
}

/* Private members */
void FFD::generateWeights()
{
  size_t x,i,j,k;
  for(x=0;x<weights.size();++x)
    {
      std::vector<float> sw,tw,uw;
      generateWeight(geometry[x],sw,tw,uw);
      for(i=0;i<GRID_DIM;i++)
	for(j=0;j<GRID_DIM;j++)
	  for(k=0;k<GRID_DIM;k++)
	    weights[x][GRID_DIM*GRID_DIM*i+GRID_DIM*j+k]=sw[i]*tw[j]*uw[k];
    }
}

void FFD::generateWeight(const triple point,std::vector<float> &i,
			   std::vector<float> &j, std::vector<float> &k)
{
  float s,t,u;
  i.resize(4);
  j.resize(4);
  k.resize(4);
  // This class always uses orthogonal S,T,U, and so finding s,t,u is 
  // simplified to reverse linear interpolation.

  s=rlerp(O.x,O.x+S.x,point.x);
  i[0]=-pow(s,3)+3*pow(s,2)-3*s+1;
  i[1]=3*pow(s,3)-6*pow(s,2)+3*s;
  i[2]=-3*pow(s,3)+3*pow(s,2);
  i[3]=pow(s,3);

  t=rlerp(O.y,O.y+T.y,point.y);
  j[0]=-pow(t,3)+3*pow(t,2)-3*t+1;
  j[1]=3*pow(t,3)-6*pow(t,2)+3*t;
  j[2]=-3*pow(t,3)+3*pow(t,2);
  j[3]=pow(t,3);

  u=rlerp(O.z,O.z+U.z,point.z);
  k[0]=-pow(u,3)+3*pow(u,2)-3*u+1;
  k[1]=3*pow(u,3)-6*pow(u,2)+3*u;
  k[2]=-3*pow(u,3)+3*pow(u,2);
  k[3]=pow(u,3);
}
/*
void FFD::generateOutput()
{
  size_t x,i,j,k,
    max;
  max=output.size();
  for(x=0;x<max;x++)
    {
      triple tI(0,0,0);
      for(i=0;i<GRID_DIM;++i)
	{
	  triple tJ(0,0,0);
	  for(j=0;j<GRID_DIM;++j)
	    {
	      triple tK(0,0,0);
	      for(k=0;k<GRID_DIM;++k)
		{
		  tK=tK+
		    controlPoints[GRID_DIM*GRID_DIM*i+GRID_DIM*j+k]*
		    weights[x][GRID_DIM*GRID_DIM*i+GRID_DIM*j+k].z;
		    
		}
	      tJ=tJ+tK*weights[x][GRID_DIM*GRID_DIM*i+GRID_DIM*j+1].y;
	    }
	  tI=tI+tJ*weights[x][GRID_DIM*GRID_DIM*i+5].x;
	}
      output[x]=tI;
    }
  output_iscached=true;
}
*/

void FFD::generateOutput() const
{
  size_t i,j,maxi;
  maxi=weights.size();
  for(i=0;i<maxi;++i)
    {
      output[i](0,0,0);
      for(j=0;j<GRID_SIZE;++j)
	{
	  output[i]=output[i]+controlPoints[j]*weights[i][j];
	}
    }
  output_iscached=true;
}
/*
float FFD::bernstein_basis(float x,unsigned n)
{
  static const std::vector<unsigned> coeff = FFD::binomial_coefficients(GRID_DIM);
  return coeff[n]*pow((1.0f-x),GRID_DIM-n)*pow(x,n);
}
*/
 /*
std::vector<unsigned> FFD::binomial_coefficients(unsigned n)
{
  unsigned i;
  std::vector<unsigned> ret;
  ret.resize(n);
  for(i=0;i<n/2;i++)
    {
      unsigned r=1, k=i+1, j=n, d;
      for (d=1; d < k; d++)
	{
	  r *= j--;
	  r /= d;
	}
      ret[i]= r;
      ret[n-k]=r;
    }
  return ret;
}
 */
/* Internal helpers */
