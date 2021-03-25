// arcball.cpp


#include "headers.h"
#include "arcball.h"




// Set the initial V matrix

void Arcball::setV( vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp )

{
  distToCentre = (initEyeLookat - initEyePosition).length();
    
  // Compute initial world-to-view transform
    
  vec3 z = (initEyePosition - initEyeLookat).normalize();
  vec3 x = (initEyeUp ^ z).normalize();
  vec3 y = (z ^ x).normalize();
  
  V.rows[0] = vec4( x, -1 * initEyePosition * x );
  V.rows[1] = vec4( y, -1 * initEyePosition * y );
  V.rows[2] = vec4( z, -1 * initEyePosition * z );
  V.rows[3] = vec4( 0, 0, 0, 1 );
}



// Centre the viewpoint at the origin

void Arcball::centreViewpoint()

{
  distToCentre = 0;
    
  V = identity4();
}



// Get only the rotational component of V (useful for cube maps)


mat4 Arcball::VRotationOnly()

{
  mat4 V2 = V;

  V2.rows[0].w = 0;
  V2.rows[1].w = 0;
  V2.rows[2].w = 0;

  return V2;
}


// Get the eye position

vec3 Arcball::eyePosition()

{
  vec4 t( V.rows[0].w, V.rows[1].w, V.rows[2].w, 0 );

  vec4 t2 = -1 * (t.x * V.rows[0] + t.y * V.rows[1] + t.z * V.rows[2]);

  return vec3( t2.x, t2.y, t2.z ); // do not do division by w! 
}


vec3 Arcball::viewDirection()

{
  return vec3( -V.rows[2].x, -V.rows[2].y, -V.rows[2].z );
}


vec3 Arcball::upDirection()

{
  return vec3( V.rows[1].x, V.rows[1].y, V.rows[1].z );
}


// Upon a mouse press, record the current state (V) and this initial
// mouse position.

void Arcball::mousePress( int button, float x, float y )

{
  currentButton = button;
  initMousePos = normalizeMousePos( x, y );
  initV = V;

  // If the initial (x,y) was outside an ellipse that almost touches
  // the window edges, rotate around the z axis

  doingZRotation = (initMousePos.length() > 0.9);
}


// Upon a mouse drag, find the offset from the initial mouse position
// and calculate the corresponding change in V.

void Arcball::mouseDrag( float x, float y )

{
  vec2 mousePos = normalizeMousePos(x,y);
  
  if (doingZRotation) {

    float angle = atan2( mousePos.y, mousePos.x ) - atan2( initMousePos.y, initMousePos.x ); 
    V = rotate( angle, vec3(0,0,1) ) * initV;

  } else {

    vec3 fromDir( initMousePos.x, initMousePos.y, 1 );
    vec3 toDir( mousePos.x, mousePos.y, 1 );

    float angle;
    vec3 axis;

    getRotation( fromDir, toDir, angle, axis );

    V = translate( vec3(0,0,-distToCentre) ) * rotate( angle, axis ) * translate( vec3(0,0,distToCentre) ) * initV; 
  }
}


// Upon a mouse scroll, translate in the view direction.
//
// Tranlate the view position forward or backward along the z axis by
// an amount proportional to the distance to the centre point.

void Arcball::mouseScroll( float xoffset, float yoffset )

{
  const float factor = 1.1;
  
  float newDistance = (yoffset < 0) ? distToCentre/factor : distToCentre*factor;
  
  V = translate( 0, 0, (distToCentre - newDistance) ) * V;

  distToCentre = newDistance;
}



// Convert mouse position to [-1,1]x[-1,1]

vec2 Arcball::normalizeMousePos( float x, float y )

{
  int width, height;
  glfwGetWindowSize( window, &width, &height );

  float w = width/2.0;
  float h = height/2.0;

  return vec2( (x - w)/w, (h - y)/h ); // flip in y direction
}


// Get the rotation between two directions in angle/axis form


void Arcball::getRotation( vec3 from, vec3 to, float &angle, vec3 &axis )

{
  vec3 cross = from ^ to;

  angle = asin( cross.length() / (from.length()*to.length()) );
  axis = cross.normalize();
}
