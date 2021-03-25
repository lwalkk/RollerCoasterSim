// train.cpp

#include "train.h"
#include "main.h"


#define SPHERE_RADIUS 5.0
#define SPHERE_COLOUR 238/255.0, 106/255.0, 20/255.0

// Draw the train.
//
// 'flag' is toggled by pressing 'F' and can be used for debugging

 
void Train::draw( mat4 &WCStoVCS, mat4 &WCStoCCS, vec3 lightDir, bool flag )

{
  
  float t = spline->paramAtArcLength( pos );

  // Draw sphere
  
  vec3 o, x, y, z;
  spline->findLocalSystem( t, o, x, y, z );
  
  height = o.z;


  mat4 M   = translate( o ) * scale( 2 * SPHERE_RADIUS, 2 * SPHERE_RADIUS, 2 * SPHERE_RADIUS );
  mat4 MV  = WCStoVCS * M;
  mat4 MVP = WCStoCCS * M;

  cube->Draw( MV, MVP, lightDir, vec3( SPHERE_COLOUR ) );

}


void Train::advance( float elapsedSeconds )

{ 
    float t = spline->paramAtArcLength(pos);

    vec3 o, x, y, z;
    spline->findLocalSystem( t, o, x, y, z );

    float zComp = z * vec3(0, 0, 1);

    accel = G - 0.2 * speed;
    
    if (speed < 20)
        speed = 20;
    else
        speed +=  zComp * accel * elapsedSeconds;


    pos += speed * elapsedSeconds;

}
