#ifndef interpolation_hpp_guard
#define interpolation_hpp_guard
inline float lerp(float a, float b, float T)
{
  return T*b+(1-T)*a;
}

// Reverse linear interpolation.
// 0<=rlerp(a,b,c)<=1 iff a<=c<=b
inline float rlerp(float a, float b, float c)
{
  if(a>c || b<c)
    throw false;
  // c=a*(1-t)+b*t
  // c=a-a*t+b*t
  // b*t-a*t=c-a
  // t(b-a)=c-a
  return (c-a)/(b-a);
}
#endif //ifndef interpolation_hpp_guard
