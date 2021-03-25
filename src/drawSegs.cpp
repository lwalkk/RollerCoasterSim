// drawSegs.cpp
//
// Self-contained function to draw a set of segments.
//
// Used mainly for debugging.
//
// Segments are drawn using the most recent MVP sent to the shaders, and the currently active shaders.


#include "headers.h"
#include "drawSegs.h"


// 'nSegs' is the number of segments.
// 'segs' is an array of nSegs vertices.

void Segs::drawSegs( GLuint primitiveType, vec3 *pts, vec3 *colours, vec3 *norms, int nPts, mat4 &MV, mat4 &MVP, vec3 lightDir )

{
  GLuint VAO;
  
  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  GLuint VBO0, VBO1, VBO2;

  // Set up points
  
  glGenBuffers( 1, &VBO0 );
  glBindBuffer( GL_ARRAY_BUFFER, VBO0 );
  glBufferData( GL_ARRAY_BUFFER, nPts * sizeof(vec3), pts, GL_STATIC_DRAW );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 0 );

  // Set up colours
  
  glGenBuffers( 1, &VBO1 );
  glBindBuffer( GL_ARRAY_BUFFER, VBO1 );
  glBufferData( GL_ARRAY_BUFFER, nPts * sizeof(vec3), colours, GL_STATIC_DRAW );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 1 );

  // Set up normals (or copy points to normals if normals are not provided, just to have some data there)

  glGenBuffers( 1, &VBO2 );
  glBindBuffer( GL_ARRAY_BUFFER, VBO2 );
  glBufferData( GL_ARRAY_BUFFER, nPts * sizeof(vec3), (norms != NULL ? norms : pts), GL_STATIC_DRAW );
  glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 2 );

  // Draw

  GLint id = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &id); // get previously-active GPU program
 
  gpuProg->activate();

  gpuProg->setMat4( "MV",  MV  );
  gpuProg->setMat4( "MVP", MVP );
  gpuProg->setVec3( "lightDir", lightDir );

  gpuProg->setInt( "useNormals", (norms != NULL) );

  glDrawArrays( primitiveType, 0, nPts );

  gpuProg->deactivate();

  glUseProgram( id ); // restore previously-active GPU program

  // Clean up
  
  glDeleteBuffers( 1, &VBO0 );
  glDeleteBuffers( 1, &VBO1 );
  glDeleteBuffers( 1, &VBO2 );

  glDisableVertexAttribArray( 0 );
  glDisableVertexAttribArray( 1 );
  glDisableVertexAttribArray( 2 );

  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  glBindVertexArray( 0 );
  glDeleteVertexArrays( 1, &VAO );
}



void Segs::drawOneSeg( vec3 tail, vec3 head, mat4 &MV, mat4 &MVP, vec3 lightDir )

{
  vec3 pts[2]    = { tail, head };
  vec3 colours[2] = { vec3(1,1,1), vec3(1,1,1) }; 

  drawSegs( GL_LINES, pts, colours, NULL, 2, MV, MVP, lightDir );
}




GPUProgram *Segs::setupShaders()

{
  GPUProgram *gpuProg = new GPUProgram();
  gpuProg->init( vertexShader, fragmentShader, "in drawSegs.cpp" );
  return gpuProg;
}


// Define basic shaders


const char *Segs::vertexShader = R"(

  #version 300 es
  
  uniform mediump mat4 MVP;
  uniform mediump mat4 MV;

  layout (location = 0) in vec4 position;
  layout (location = 1) in vec3 colour_in;
  layout (location = 2) in vec3 normal_in;

  smooth out mediump vec3 colour;
  smooth out mediump vec3 normal;
  
  void main()
  
  {
    gl_Position = MVP * position;

    colour = colour_in;
    normal = (MV * vec4( normal_in, 0 )).xyz;
  }
)";


const char *Segs::fragmentShader = R"(

  #version 300 es

  uniform mediump vec3 lightDir;
  uniform bool useNormals;
  
  smooth in mediump vec3 colour;
  smooth in mediump vec3 normal;

  out mediump vec4 fragColour;
  
  void main()
  
  {
    mediump float ndotl;

    if (!useNormals)
      ndotl = 1.0;
    else {
      ndotl = dot( lightDir, normalize(normal) );
      if (ndotl < 0.1)
        ndotl = 0.1;
    }

    fragColour = vec4( ndotl * colour, 1 );
  }
)";
