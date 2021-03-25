// ctrlPoints.h


#ifndef CTRLPOINTS_H
#define CTRLPOINTS_H

#include "headers.h"
#include "seq.h"
#include "spline.h"


class CtrlPoints {

 public:

  seq<vec3> points;             // points in the air
  seq<vec3> bases;              // base on the terrain

  Spline *spline;

  GLFWwindow *window;

  CtrlPoints( Spline *s, GLFWwindow *w ) {
    spline = s;
    window = w;
  }

  CtrlPoints( GLFWwindow *w ) {
    window = w;
  }

  void clear() {
    points.clear();
    bases.clear();
    spline->clear();
  }

  int count() {
    return points.size();
  }

  void draw( bool drawPostsOnly, mat4 &WCStoVCS, mat4 &WCStoCCS, vec3 lightDir, vec3 colour );
  void addPoint( vec3 v );
  void addPointWithHeight( vec3 v, float height );
  void deletePoint( int index );
  void moveBase( int index, vec3 newPos );
  void setHeight( int index, float height );
  int findSelectedPoint( vec3 start, vec3 dir, mat4 &M );

  float maxHeight() {
    float max = -MAXFLOAT;
    for (int i=0; i<points.size(); i++)
      if (points[i].z > max)
        max = points[i].z;
    return max;
  }
};


#endif
