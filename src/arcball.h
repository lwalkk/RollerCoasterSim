// arcball.h
//
// Arcball interface to move viewpoint around.
//
// Drag mouse with left button to rotate viewpoint around centre.
// Scroll mouse to zoom.
//
// Your code has to call mousePress, mouseDrag, and mouseScroll when
// these events occur.  Your code has to use the arcball's V matrix as
// the world-to-view transformation.


#ifndef ARCBALL_H
#define ARCBALL_H

#include "headers.h"
#include "linalg.h"


class Arcball {

  int currentButton;  // button down during drag
  vec2 initMousePos;  // initial (x,y) at button down
  mat4 initV;         // initial V at button down

  GLFWwindow *window;

  bool doingZRotation;  // true if rotating about viewing z axis
  
  vec2 normalizeMousePos( float x, float y );
  void getRotation( vec3 from, vec3 to, float &angle, vec3 &axis );

 public:

  mat4 V;  // world-to-view transform
  float distToCentre; // distance to the 'look at' point

  Arcball( GLFWwindow *w ) {
    window = w;
  }
  
  Arcball( GLFWwindow *w, vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp ) {
    window = w;
    setV( initEyePosition, initEyeLookat, initEyeUp );
  }

  void mousePress( int button, float x, float y );
  void mouseDrag( float x, float y );
  void mouseScroll( float xoffset, float yoffset );

  void setV( vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp );
  void centreViewpoint();

  mat4 VRotationOnly();
  vec3 eyePosition();
  vec3 upDirection();
  vec3 viewDirection();
};

#endif
