// axes.h
//
// Set up and draw a set of (r,g,b) axes


#ifndef AXES_H
#define AXES_H


#include "linalg.h"
#include "gpuProgram.h"


class Axes {

  GLuint VAO;
  GPUProgram program;
  static const char *vertShader;
  static const char *fragShader;

 public:

  Axes();
  void draw( mat4 &MVP_transform );
};


#endif
