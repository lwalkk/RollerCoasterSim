/* sphere.C
 */


#include "sphere.h"


// icosahedron vertices (taken from Jon Leech http://www.cs.unc.edu/~jon)

vec3 Sphere::icosahedronVerts[NUM_VERTS] = {
  vec3(  tau,  one,    0 ),
  vec3( -tau,  one,    0 ),
  vec3( -tau, -one,    0 ),
  vec3(  tau, -one,    0 ),
  vec3(  one,   0 ,  tau ),
  vec3(  one,   0 , -tau ),
  vec3( -one,   0 , -tau ),
  vec3( -one,   0 ,  tau ),
  vec3(   0 ,  tau,  one ),
  vec3(   0 , -tau,  one ),
  vec3(   0 , -tau, -one ),
  vec3(   0 ,  tau, -one )
};


// icosahedron faces (taken from Jon Leech http://www.cs.unc.edu/~jon)

int Sphere::icosahedronFaces[NUM_FACES][3] = {
  { 4, 8, 7 },
  { 4, 7, 9 },
  { 5, 6, 11 },
  { 5, 10, 6 },
  { 0, 4, 3 },
  { 0, 3, 5 },
  { 2, 7, 1 },
  { 2, 1, 6 },
  { 8, 0, 11 },
  { 8, 11, 1 },
  { 9, 10, 3 },
  { 9, 2, 10 },
  { 8, 4, 0 },
  { 11, 0, 5 },
  { 4, 9, 3 },
  { 5, 3, 10 },
  { 7, 8, 1 },
  { 6, 1, 11 },
  { 7, 2, 9 },
  { 6, 10, 2 },
};


// Add a level to the sphere

void Sphere::refine()

{
  int n = faces.size();

  for (int i=0; i<n; i++) {

    SphereFace f = faces[i];

    verts.add( (verts[ f.v[0] ] + verts[ f.v[1] ]).normalize() );
    verts.add( (verts[ f.v[1] ] + verts[ f.v[2] ]).normalize() );
    verts.add( (verts[ f.v[2] ] + verts[ f.v[0] ]).normalize() );

    int v01 = verts.size() - 3;
    int v12 = verts.size() - 2;
    int v20 = verts.size() - 1;

    faces.add( SphereFace( f.v[0], v01, v20 ) );
    faces.add( SphereFace( f.v[1], v12, v01 ) );
    faces.add( SphereFace( f.v[2], v20, v12 ) );

    faces[i].v[0] = v01;
    faces[i].v[1] = v12;
    faces[i].v[2] = v20;
  }
}


void Sphere::setupVAO()

{
  // Set up buffers of vertices, normals, and face indices.  These are
  // collected from the sphere's 'seq<vec3> verts' and
  // 'seq<SphereFace> faces' structures.

  // copy vertices

  int nVerts = verts.size();

  GLfloat *vertexBuffer = new GLfloat[ nVerts * 3 ];

  for (int i=0; i<nVerts; i++)
    ((vec3 *) vertexBuffer)[i] = verts[i];

  // copy normals
  //
  // Since the vertices are on a sphere centred at the origin, and are
  // distance 1 from the origin, the normals are the same as the
  // vertices.

  GLfloat *normalBuffer = new GLfloat[ nVerts * 3 ];

  for (int i=0; i<nVerts; i++)
    ((vec3 *) normalBuffer)[i] = verts[i];

  // copy faces

  int nFaces = faces.size();

  GLuint *indexBuffer = new GLuint[ nFaces * 3 ];

  for (int i=0; i<faces.size(); i++)
    for (int j=0; j<3; j++) 
      indexBuffer[3*i+j] = faces[i].v[j]; 

  // Create a VAO

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // store vertices (i.e. one triple of floats per vertex)

  GLuint vertexBufferID;
  glGenBuffers( 1, &vertexBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, vertexBufferID );

  glBufferData( GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), vertexBuffer, GL_STATIC_DRAW );

  // attribute 0 = position

  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // store vertex normals (i.e. one triple of floats per vertex)

  GLuint normalBufferID;
  glGenBuffers( 1, &normalBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, normalBufferID );

  glBufferData( GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), normalBuffer, GL_STATIC_DRAW );

  // attribute 1 = normal

  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // store faces (i.e. one triple of vertex indices per face)

  GLuint indexBufferID;
  glGenBuffers( 1, &indexBufferID );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferID );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, nFaces * 3 * sizeof(GLuint), indexBuffer, GL_STATIC_DRAW );

  // Clean up

  delete[] vertexBuffer;
  delete[] normalBuffer;
  delete[] indexBuffer;
}


void Sphere::draw( mat4 &MV, mat4 &MVP, vec3 lightDir, vec3 colour )

{
  gpu.activate();
  
  gpu.setMat4( "MV", MV );
  gpu.setMat4( "MVP", MVP );
  gpu.setVec3( "colour", colour );
  gpu.setVec3( "lightDir", lightDir );
  
  // Draw using element array

  glBindVertexArray( VAO );
  glDrawElements( GL_TRIANGLES, faces.size()*3, GL_UNSIGNED_INT, 0 );
  glBindVertexArray( 0 );

  gpu.deactivate();
}


const char *Sphere::vertShader = R"(

  #version 300 es

  uniform mat4 MVP;
  uniform mat4 MV;

  layout (location = 0) in mediump vec3 vertPosition;
  layout (location = 1) in mediump vec3 vertNormal;

  smooth out mediump vec3 normal;

  void main() {

    gl_Position = MVP * vec4( vertPosition, 1.0 );
    normal = vec3( MV * vec4( vertNormal, 0.0 ) );
  }
)";


const char *Sphere::fragShader = R"(

  #version 300 es

  uniform mediump vec3 colour;
  uniform mediump vec3 lightDir;

  smooth in mediump vec3 normal;
  out mediump vec4 outputColour;

  void main() {

    mediump float NdotL = dot( normalize(normal), lightDir );

    if (NdotL < 0.0)
      NdotL = 0.1; // some ambient

    outputColour = vec4( NdotL * colour, 1.0 );
  }
)";





