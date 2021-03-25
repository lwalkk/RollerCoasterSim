// drawSegs.h
//
// Create a class instance:
//
//    Segs *segs = new Segs();
//
// Use it:
//
//    segs->drawOneSeg( tail, head, MVP );


#ifndef DRAW_SEGS_H
#define DRAW_SEGS_H

#include "headers.h"
#include "gpuProgram.h"


class Segs {

  static const char *fragmentShader;
  static const char *vertexShader;

  GPUProgram *setupShaders();

  GPUProgram *gpuProg;
  
 public:

  Segs() { 
    gpuProg = setupShaders();
  };
  
  void drawSegs( GLuint primitiveType, vec3 *pts, vec3 *colours, vec3 *norms, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir );


  void drawSegs( GLuint primitiveType, vec3 *pts, vec3 *colours, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir ) {
    drawSegs( primitiveType, pts, colours, NULL, nPts, MV, MVP, lightDir );
  }

  void drawOneSeg( vec3 tail, vec3 head, mat4 &MV, mat4 &MVP, vec3 lightDir );
};

#endif
