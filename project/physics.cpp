#include "physics.hpp"
#include "../shared/triple.hpp"
#include "../shared/FFD.hpp"
#include "../shared/interpolation.hpp"
// Reccomended by Marshal Cline's C++ FAQ
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember)) 



// Consts

static const float SPRING_CONSTANT=.25, DAMP_CONSTANT=0.4;

// Public members

physics::physics(FFD &_grid, float _h):
  RKfunc(NULL),grid(_grid),h(_h),
  s(_grid.getS().x/3),t(_grid.getT().y/3),u(_grid.getU().z/3),
  posBuff(),velBuff(),accelBuff(),
  velocities(),accelerations(),
  positions(_grid.getControlPoints())
{
  posBuff.resize(64);
  velBuff.resize(64);
  accelBuff.resize(64);
  
  velocities.resize(64);
  accelerations.resize(64);
  
  RKfunc=&physics::springphys;

  simreset();
}

void physics::simstep(float t)
{
  std::vector<float> args,properties;
  args.resize(9);

  for(_i=0;_i<4;++_i)
    for(_j=0;_j<4;++_j)
      for(_k=0;_k<4;++_k)  
	{
	  for(size_t i=0;i<3;++i)
	    args[i]=positions[16*_i+4*_j+_k][i];
	  for(size_t i=0;i<3;++i)
	    args[i+3]=velocities[16*_i+4*_j+_k][i];
	  for(size_t i=0;i<3;++i)
	    args[i+6]=accelerations[16*_i+4*_j+_k][i];

	  properties=RK2(t,args);

	  for(size_t i=0;i<3;++i)
	    posBuff[16*_i+4*_j+_k][i]=properties[i];
	  for(size_t i=0;i<3;++i)
	    velBuff[16*_i+4*_j+_k][i]=properties[i+3];
	  for(size_t i=0;i<3;++i)
	    accelBuff[16*_i+4*_j+_k][i]=properties[i+6];
	}
  flush_buffs();
}

void physics::simreset()
{
  std::vector<triple>::iterator itor;
  size_t i;
  for(itor=velocities.begin();itor!=velocities.end();++itor)
    (*itor)=triple(0,0,0);
  for(itor=accelerations.begin();itor!=accelerations.end();++itor)
    (*itor)=triple(0,0,0);

  // Flat front face impact
  for(i=0;i<16;++i)
    velocities[i] = triple(1,0,0);

  // Corner impact
  /*
    velocities[0]= triple(1,1,1);
   */
  // Shaped front face impact
  /*
    for(i=0;i<16;++i)
    velocities[i]= triple(1,0,0);

    velocities[6]= triple(2,0,0);
    velocities[7]= triple(2,0,0);
    velocities[10]=triple(2,0,0);
    velocities[11]=triple(2,0,0);
   */

  // Internal explosion
  /*
    velocities[37]=triple(-1,-1,-1);
    velocities[38]=triple(1,-1,-1);
    velocities[41]=triple(-1,1,-1);
    velocities[42]=triple(1,1,-1)
;
    velocities[53]=triple(-1,-1,1);
    velocities[54]=triple(1,-1,1);
    velocities[57]=triple(-1,1,1);
    velocities[58]=triple(1,1,1);
   */
}


// Private members
std::vector<float> physics::RK2(float t, const std::vector<float>& x)
{
  size_t i,max=x.size();
  std::vector<float> a,b,ret=x;
  a = K1(t,x);
  b = K2(t,x,a);

  for(i=0;i<max;i++)
    {
      ret[i]=x[i]+(a[i]*0.5+b[i]*0.5)*h;
    }
  return ret;
}

std::vector<float> physics::K1(float t, const std::vector<float>& x)
{
  if(!RKfunc)
    throw std::logic_error("physics::K1(float, const std::vector<float>&): RKfunc is NULL");

  return CALL_MEMBER_FN(*this,RKfunc)(t,x);
}

std::vector<float> physics::K2(float t, const std::vector<float>& x, const std::vector<float> &a)
{
  if(!RKfunc)
    throw std::logic_error("physics::K2(float, const std::vector<float>&): RKfunc is NULL");

  size_t itor,max=x.size();
  std::vector<float> args=x;
  for(itor=0;itor<max;++itor)
    args[itor]+=a[itor]*.5*h;

  return CALL_MEMBER_FN(*this,RKfunc)(t+.5*h,args);//x+K1(t,x)*.5*h
}



std::vector<float> physics::springphys(float t, const std::vector<float>& x)
{
  // Springs connect points where exactly 1 of i,j,k differ by 1.
  // F=-k(a-b)/m
  // where a is the current position and b is the rest position.
  // k is arbitrary, so assume m=1 for simplicity

  std::vector<float> ret;
  ret.resize(x.size());

  const triple // Split up the input vector
    p(x[0],x[1],x[2]), //initial position
    v(x[3],x[4],x[5]), //initial velocity
    a(x[6],x[7],x[8]); //initial acceleration
  size_t i;

  // Instantaneous derivitive of position (= to current velocity)
  for(i=0;i<3;++i)
    ret[i]=v[i];
  // Instantaneous derivative of velocity (= to current acceleration)
  for(i=3;i<6;++i)
    ret[i]=a[i-3];

  // Time for the real meat: the instantaneous derivative of acceleration!

  /* a=kd/m - fv/m
     where k is the spring constant, m is mass, d is the distance between
     current position (p) and rest position (r), and f is the frictional 
     constant. Assume m=1. THEREFORE:
     a' = (kd-fv)' = (k(r-p)-fv)' = (kr-kp-fv)' = -k(p')-f(v') = -kv-fa
     But wait! There's more!
     There are 6 potential springs per point, each with their own relative 
     velocity! And not every point has all six springs! OH BOY.
  */

  for(i=6;i<9;++i)
    ret[i]=0;
  if(_i<3)
    {
      // get relative velocity
      triple rv=v-velocities[16*(_i+1)+4*_j+_k];
      for(i=6;i<9;++i)
	ret[i]-=rv[i-6]*SPRING_CONSTANT;
    }
  if(_i>0)
    {
      // get relative velocity
      triple rv=v-velocities[16*(_i-1)+4*_j+_k];
      for(i=6;i<9;++i)
	ret[i]-=rv[i-6]*SPRING_CONSTANT;
    }
  
  if(_j<3)
    {
      // get relative velocity
      triple rv=v-velocities[16*_i+4*(_j+1)+_k];
      for(i=6;i<9;++i)
	ret[i]-=rv[i-6]*SPRING_CONSTANT;
    }
  if(_j>0)
    {
      // get relative velocity
      triple rv=v-velocities[16*_i+4*(_j-1)+_k];
      for(i=6;i<9;++i)
	ret[i]-=rv[i-6]*SPRING_CONSTANT;
    }
  
  if(_k<3)
    {
      // get relative velocity
      triple rv=v-velocities[16*_i+4*_j+_k+1];
      for(i=6;i<9;++i)
	ret[i]-=rv[i-6]*SPRING_CONSTANT;
    }
  if(_k>0)
    {
      // get relative velocity
      triple rv=v-velocities[16*_i+4*_j+_k-1];
      for(i=6;i<9;++i)
	ret[i]-=rv[i-6]*SPRING_CONSTANT;
    }

  // "Friction" so that our errors don't build up into infinite velocities

  for(i=6;i<9;++i)
    ret[i]-=x[i]*DAMP_CONSTANT;
  
  return ret;
}

void physics::flush_buffs(void)
{
 for(_i=0;_i<4;++_i)
    for(_j=0;_j<4;++_j)
      for(_k=0;_k<4;++_k)
	{
	  grid.setPoint(16*_i+4*_j+_k,posBuff[16*_i+4*_j+_k]);
	}
  velocities=velBuff;
  accelerations=accelBuff;
  
}
