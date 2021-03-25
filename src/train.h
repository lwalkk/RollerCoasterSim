// train.h

#ifndef TRAIN_H
#define TRAIN_H

#include "headers.h"
#include "spline.h"


#define SPEED_INC 0.5
#define G 9.81

class Train {

  Spline *spline;

  // state

  float accel;
  float pos;                    // position on spline
  float speed;

  float mass;
  float height;

 public:

  Train( Spline *spl ) {
    spline = spl;
    pos = 0;
    speed = 70;
    mass = 1;
  }
  
  void draw( mat4 &WCStoVCS, mat4 &WCStoCCS, vec3 lightDir, bool flag ); 
  void advance( float elapsedSeconds );

  float getSpeed() {
    return speed;
  }

  void accelerate() {
    speed += SPEED_INC;
  }

  void brake() {
    speed -= SPEED_INC;
  }
};



#endif
