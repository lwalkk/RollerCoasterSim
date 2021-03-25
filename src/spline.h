/* spline.h
 *
 * This is a one-dimensional spline with a fixed knot vector [0,1,2,...].
 */


#ifndef SPLINE_H
#define SPLINE_H

#include "headers.h"
#include "seq.h"

#define DIVS_PER_SEG 20    
#define SPLINE_COLOUR vec3(0.8,0.9,0.5)

enum evalType { VALUE, TANGENT };

  

class Spline {

  static float M[][4][4];       // change-of-basis matrices
  static const char * MName[];  // names of the matrices

  int currSpline;

  void computeArcLengthParameterization();
  float *arcLength;
  float maxHeight;

 public:

  seq<vec3> data;               // the data points
  bool mustRecomputeArcLength;

  Spline() {
    mustRecomputeArcLength = true;
    arcLength = NULL;
    currSpline = 0;
  }

  void clear() {
    data.clear();
    mustRecomputeArcLength = true;
  }

  void nextCOB() {
    currSpline++;
    if (MName[currSpline][0] == '\0')
      currSpline = 0;
    mustRecomputeArcLength = true;
  }

  const char *name() {
    return MName[currSpline];
  }

  float getMaxHeight() {
    if (mustRecomputeArcLength)
      computeArcLengthParameterization();
    return maxHeight;
  }

  void draw( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawIntervals );
  void drawWithArcLength( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawIntervals );
  void addPoint( vec3 v );
  float paramAtArcLength( float s );
  float totalArcLength();

  void findLocalSystem( float t, vec3 &o, vec3 &x, vec3 &y, vec3 &z );
  mat4 findLocalTransform( float t );
  void drawLocalSystem( float t, mat4 &MVP );

  vec3 eval( float t, evalType type ); // evaluate the spline at param t

  vec3 value( float t ) {
    return eval( t, VALUE );
  }

  vec3 tangent( float t ) {
    return eval( t, TANGENT );
  }
};

#endif
