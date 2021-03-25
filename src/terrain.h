// terrain.h

#ifndef TERRAIN_H
#define TERRAIN_H

#include "headers.h"
#include "texture.h"
#include "seq.h"
#include "gpuProgram.h"


class Terrain {

  vec3 **points;
  vec3 **normals;
  seq<vec3> quadsToHighlight;

  bool rayTriangleInt( vec3 rayStart, vec3 rayDir, vec3 v0, vec3 v1, vec3 v2, vec3 & intPoint, float & intParam );

  GLuint      VAO; 
  GPUProgram  gpu;
  int         nFaces;

  static const char *vertShader;
  static const char *fragShader;

  float elapsedSeconds;

 public:

  Texture *heightfield;
  Texture *texture;

  Terrain( string basePath, string heightfieldFilename, string textureFilename ) {
    readTextures( basePath, heightfieldFilename, textureFilename );
    gpu.init( vertShader, fragShader, "in terrain.cpp" );
    setupVAO();
  }
  
  void readTextures( string basePath, string heightfieldFilename, string textureFilename );
  void setupVAO();
  void draw( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawUndersideOnly );

  inline void setTime(float time) { elapsedSeconds = time; }

  bool findIntPoint( vec3 rayStart, vec3 rayDir, vec3 planePerp, vec3 &intPoint, mat4 &M );
};

#endif
