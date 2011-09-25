#ifndef PHYSICS_GUARD
#define PHYSICS_GUARD
#include <vector>

class triple;
class FFD;
class physics
{
public:
  physics(FFD &_grid, float _h=.25);

  void simstep(float t=0);
  void simreset();
protected:
private:
  std::vector<float> RK2(float, const std::vector<float>&);
  std::vector<float> K1 (float, const std::vector<float>&);
  std::vector<float> K2 (float, const std::vector<float>&,
			 const std::vector<float>&);

  std::vector<float> springphys(float, const std::vector<float>&);

  void flush_buffs(void);

  // For correct operation, RKfunc must return a vector of equal size to
  // the second parameter.
  std::vector<float> (physics::*RKfunc)(float,const std::vector<float>&);

  FFD &grid;
  size_t _i,_j,_k;
  float h;
  const float s,t,u;
  std::vector<triple> posBuff, velBuff, accelBuff,
    velocities, accelerations;
  const std::vector<triple> & positions;

};

#endif // ifndef PHYSICS_GUARD
