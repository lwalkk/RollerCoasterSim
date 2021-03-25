// ctrlPoints.cpp

#include "ctrlPoints.h"
#include "main.h"


#define POST_RADIUS 2.0
#define POINT_RADIUS 4.0
#define INITIAL_VERTICAL_OFFSET vec3(0,0,30)
#define MAX(a,b)  ((a)>(b)?(a):(b))


void CtrlPoints::draw( bool drawPostsOnly, mat4 &WCStoVCS, mat4 &WCStoCCS, vec3 lightDir, vec3 colour )

{
  mat4 M, MV, MVP;
  
  for (int i=0; i<points.size(); i++) {

    if (!drawPostsOnly) {

      // draw base

      M   = translate( bases[i] ) * scale( POINT_RADIUS, POINT_RADIUS, POINT_RADIUS );
      MV  = WCStoVCS * M;
      MVP = WCStoCCS * M;

      sphere->draw( MV, MVP, lightDir, colour );

      // draw top

      M   = translate( points[i] ) * scale( POINT_RADIUS, POINT_RADIUS, POINT_RADIUS );
      MV  = WCStoVCS * M;
      MVP = WCStoCCS * M;

      sphere->draw( MV, MVP, lightDir, colour );
    }

    // draw post

    float len = points[i].z - bases[i].z;

    M   = translate( bases[i] ) * scale( POST_RADIUS, POST_RADIUS, len ) * translate( 0, 0, 0.5 );
    MV  = WCStoVCS * M;
    MVP = WCStoCCS * M;

    cylinder->draw( MV, MVP, lightDir, colour );
  }
}


void CtrlPoints::addPoint( vec3 pt )

{
  // Find the closest edge to v

  float minDist = MAXFLOAT;
  int edgeIndex = -1;

  for (int i=0; i<bases.size(); i++) {
    int j = (i+1) % bases.size();
    vec3 u = pt - bases[i];
    vec3 v = (bases[j] - bases[i]).normalize();
    float t = u*v;
    if (t >= 0 && t <= (bases[j]-bases[i]).length()) {

      float dist = (u - t*v).squaredLength();

      if (dist < minDist) {
        minDist = dist;
        edgeIndex = i;
      }
    }
  }

  // Insert v into this closest edge

  edgeIndex++;

  if (edgeIndex < bases.size()) {

    bases.shift( edgeIndex );
    bases[edgeIndex] = pt;

    points.shift( edgeIndex );
    points[edgeIndex] = pt + INITIAL_VERTICAL_OFFSET;

    spline->data.shift( edgeIndex );
    spline->data[edgeIndex] = points[edgeIndex];

  } else {

    bases.add( pt );
    points.add( pt + INITIAL_VERTICAL_OFFSET );
    spline->data.add( points[edgeIndex] );
  }

  spline->mustRecomputeArcLength = true;
}


void CtrlPoints::addPointWithHeight( vec3 v, float height )

{
  bases.add( v );
  points.add( v + vec3(0,0,height) );
  spline->data.add( points[points.size()-1] );
  spline->mustRecomputeArcLength = true;
}


void CtrlPoints::deletePoint( int index )

{
  bases.remove(index);
  points.remove(index);
  spline->data.remove(index);
  spline->mustRecomputeArcLength = true;
}


void CtrlPoints::moveBase( int index, vec3 newPos )

{
  float z = points[index].z;
  
  bases[index] = newPos;

  newPos.z = z;                 // keep the original height
  points[index] = newPos;
  spline->data[index] = newPos;
  spline->mustRecomputeArcLength = true;
}


void CtrlPoints::setHeight( int index, float height )

{
  points[index].z = bases[index].z + height;
  spline->data[index] = points[index];
  spline->mustRecomputeArcLength = true;
}



// Find the control point intersected by the line (start + t * dir)
//
// Return -1 if not found.  Assume that |dir| = 1.


int CtrlPoints::findSelectedPoint( vec3 start, vec3 dir, mat4 &M )

{
  int   minIndex = -1;
  float minZDist = MAXFLOAT;

  for (int i=0; i<points.size(); i++) {

    // base i

    vec3 p = (M * vec4( bases[i], 1 )).toVec3();
    
    float distToLine = p.distanceToLine( start, dir );

    if (distToLine < POINT_RADIUS) {
      float zdist = (p - start)*dir;
      if (zdist < minZDist) {
        minZDist = zdist;
        minIndex = 2*i + 0;  // ID of base of i is 2i
      }
    }

    // top i

    p = (M * vec4( points[i], 1 )).toVec3();

    distToLine = p.distanceToLine( start, dir );

    if (distToLine < POINT_RADIUS) {
      float zdist = (p - start)*dir;
      if (zdist < minZDist) {
        minZDist = zdist;
        minIndex = 2*i + 1;  // ID of base of i is 2i+1
      }
    }
  }

  return minIndex;
}
