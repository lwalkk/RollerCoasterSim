// cylinder.h
//
// cylinder with radius 1 in xy plane and height 1 in z direction (between -0.5 and +0.5)


#ifndef CYLINDER_H
#define CYLINDER_H


#include "linalg.h"
#include "seq.h"
#include "gpuProgram.h"


class CylinderFace {
 public:
  unsigned int v[3];
  CylinderFace() {}
  CylinderFace( int v0, int v1, int v2 ) {
    v[0] = v0; v[1] = v1; v[2] = v2;
  }
};


class Cylinder {

 public:

  Cylinder( int numSlices ) {

    // bottom vertices
    // add centre points / normal?

    verts.add( vec3( 0, 0, -0.5 ) );
    normals.add( vec3( 0, 0, -1 ) );
    for (int i=0; i<numSlices; i++) {
      float theta = i/(float)numSlices*2*M_PI;
      verts.add( vec3( cos(theta), sin(theta), -0.5 ) );
      normals.add( vec3( 0, 0, -1 ) );
    }

    // bottom faces
    
    for (int i=0; i<numSlices; i++)
      faces.add( CylinderFace( (i+1) % numSlices, i, 0 ) );

    // top vertices
    
    verts.add( vec3( 0, 0, +0.5 ) );
    normals.add( vec3( 0, 0, +1 ) );
    for (int i=0; i<numSlices; i++) {
      float theta = i/(float)numSlices*2*M_PI;
      verts.add( vec3( cos(theta), sin(theta), +0.5 ) );
      normals.add( vec3( 0, 0, +1 ) );
    }

    // top faces
    
    for (int i=0; i<numSlices; i++)
      faces.add( CylinderFace( i + (numSlices+1), (i+1) % (numSlices+1) + (numSlices+1), 0 + (numSlices+1) ) );

    // side vertices (same as others, but with *different* normals)

    int baseIndex = 2*(numSlices+1);
    
    for (int i=0; i<numSlices; i++) {
      float theta = i/(float)numSlices*2*M_PI;
      verts.add( vec3( cos(theta), sin(theta), -0.5 ) );
      normals.add( vec3( cos(theta), sin(theta), 0 ) );
    }

    for (int i=0; i<numSlices; i++) {
      float theta = i/(float)numSlices*2*M_PI;
      verts.add( vec3( cos(theta), sin(theta), +0.5 ) );
      normals.add( vec3( cos(theta), sin(theta), 0 ) );
    }

    // side faces
    
    for (int i=0; i<numSlices; i++) {

      faces.add( CylinderFace( baseIndex + i, 
                               baseIndex + (i+1) % numSlices, 
                               baseIndex + (i+1) % numSlices + numSlices ) );

      faces.add( CylinderFace( baseIndex + (i+1) % numSlices + numSlices, 
                               baseIndex + i + numSlices, 
                               baseIndex + i ) );
    }

    gpu.init( vertShader, fragShader, "in cylinder.cpp" );

    setupVAO();
  };

  ~Cylinder() {}

  // The cylinder drawing provides to the shader (a) the OCS vertex
  // position as attribute 0 and (b) the OCS vertex normal as
  // attribute 1.
  //
  // The cylinder is drawn at the origin with radius 1 and height 1
  // (in [-0.5,+0.5]).  You must provide mat4 MV and mat4 MVP matrices,
  // as well as vec3 lightDir and vec3 colour.
  
  void draw( mat4 &MV, mat4 &MVP, vec3 lightDir, vec3 colour );

 private:

  seq<vec3>         verts;
  seq<vec3>         normals;
  seq<CylinderFace> faces;
  GLuint            VAO; 

  GPUProgram        gpu;

  static const char *vertShader;
  static const char *fragShader;

  void setupVAO();
};

#endif
