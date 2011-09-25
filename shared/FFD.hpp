#ifndef FFD_hpp_guard
#define FFD_hpp_guard
#include <vector>
#include <stdexcept>
#include <cmath>
#include "triple.hpp"

//class glMatrixf;
class FFD
{
public:
  FFD(const std::vector<triple> &);
  ~FFD() throw();

  // Getters!
  const std::vector<triple> &getControlPoints() const;//inline
  const std::vector<triple> &getOutput() const;
  triple getOrigin() const; // inline
  triple getS() const; // inline
  triple getT() const; // inline
  triple getU() const; // inline

  // Throws if int is larger than size of control points
  void setPoint(unsigned,triple)
    throw (std::invalid_argument); // inline
protected:
  const std::vector<triple> &geometry;

  triple O,S,T,U;
  std::vector<triple> controlPoints;
  mutable std::vector<triple> output;
  std::vector< std::vector<float> > weights;
private:
  mutable bool output_iscached;

  // Calculate the control point weights from geometry
  void generateWeights();
  // Calculate and return the length 4 weight arrays for s,t,u
  void generateWeight(const triple,std::vector<float>&, std::vector<float>&,
		      std::vector<float>&);
  // Update output
  void generateOutput() const;
  // The nth row of pascal's triangle.
  //static std::vector<unsigned> binomial_coefficients(unsigned);
  // Evaluate a bernstein basis polynomial
  //float bernstein_basis(float,unsigned);
};

/* Inline public */
inline const std::vector<triple> &FFD::getControlPoints() const
{ return controlPoints; }

inline triple FFD::getOrigin() const
{ return O; }
inline triple FFD::getS() const
{ return S; }
inline triple FFD::getT() const
{ return T; }
inline triple FFD::getU() const
{ return U; }


// Throws if int is larger than size of control points
inline void FFD::setPoint(unsigned index, triple arg)
  throw (std::invalid_argument)
{
  if(index>controlPoints.size())
    throw std::invalid_argument("FFD::setPoint(unsigned,triple): unsigned is too large.");

  controlPoints[index] = arg;
  output_iscached = false;
}

/* Inline private */


#endif //ifndef FFD_hpp_guard
