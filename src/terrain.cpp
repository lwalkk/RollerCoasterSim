// terrain.cpp


#include "terrain.h"
#include "main.h"


#define CURTAIN_COLOUR 0.6,0.6,0.4
#define BOTTOM_COLOUR  0.3,0.3,0.2
#define POST_COLOUR    0.6*.7,0.6*.7,0.4*.7

#define VERTEX(x,y,z)  glVertex3f(x,y,z)

void Terrain::readTextures( string basePath, string heightfieldFilename, string textureFilename )

{
  heightfield = new Texture( basePath, heightfieldFilename );
  texture = new Texture( basePath, textureFilename );

  // Store texture map as a vec3 array.  Create a border around it
  // to allow indexing one beyond the texture.

  points = new vec3*[ heightfield->width + 2 ];
  for (unsigned int x=0; x<heightfield->width + 2; x++)
    points[x] = new vec3[ heightfield->height + 2 ];

  for (unsigned int x=0; x<heightfield->width; x++)
    for (unsigned int y=0; y<heightfield->height; y++) {
      float alpha;
      float height = heightfield->texel(x, y, alpha).x * 0.1*heightfield->width; // max height is 10% of width
      points[x][y] = vec3(x,y, 10);
    }

  // Compute normals for the texture map

  normals = new vec3*[ heightfield->width ];
  for (unsigned int x=0; x<heightfield->width; x++) {
    normals[x] = new vec3[ heightfield->height ];
    for (unsigned int y=0; y<heightfield->height; y++) {

      // average the face normals around heightfield[x][y]

      int offsets[8][2] = { {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1} };

      int count = 0;
      vec3 sum(0,0,0);
      vec3 &c = points[x][y];

      for (int i=0; i<8; i++) {

        int cwx = x+offsets[i][0];
        int cwy = y+offsets[i][1];

        int ccwx = x+offsets[(i+1)%8][0];
        int ccwy = y+offsets[(i+1)%8][1];

        if (cwx >= 0 && cwx < (int) heightfield->width &&
            cwy >= 0 && cwy < (int) heightfield->height &&
            ccwx >= 0 && ccwx < (int) heightfield->width &&
            ccwy >= 0 && ccwy < (int) heightfield->height) {

          vec3 &cw = points[cwx][cwy];
          vec3 &ccw = points[ccwx][ccwy];
          vec3 n = ((cw-c) ^ (ccw-c)).normalize();
          sum = sum + n;
          count++;
        }
      }

      normals[x][y] = (1/(float)count) * sum;
    }
  }
}



void Terrain::setupVAO()

{
  // Set up buffers of vertices, normals, and texture coordinates.

  int nVerts = heightfield->width * heightfield->height;

  GLfloat *vertexBuffer = new GLfloat[ nVerts * 3 ];
  GLfloat *normalBuffer = new GLfloat[ nVerts * 3 ];
  GLfloat *texCoordBuffer = new GLfloat[ nVerts * 2 ];

  vec3 *v = (vec3*) vertexBuffer;
  vec3 *n = (vec3*) normalBuffer;
  vec2 *t = (vec2*) texCoordBuffer;

  for (unsigned int y=0; y<heightfield->height; y++)
    for (unsigned int x=0; x<heightfield->width; x++) {
      *v++ = points[x][y];
      *n++ = normals[x][y];
      *t++ = vec2( x/(float)( 0.25 * heightfield->width-1), y/(float)(0.25 * heightfield->height-1) );
    }
      
  // set up triangular faces to cover the terrain

  nFaces = 2 * (heightfield->width - 1) * (heightfield->height - 1);

  GLuint *indexBuffer = new GLuint[ nFaces * 3 ];

  GLuint *i = indexBuffer;

  int k = 0;  // = index into v/n/t buffers of current LL corner (min x, min y) of current quad

  for (unsigned int y=0; y<heightfield->height - 1; y++) {
    for (unsigned int x=0; x<heightfield->width - 1; x++) {

      // one face
      
      *i++ = k;
      *i++ = k+1;
      *i++ = k + heightfield->width;

      // other face

      *i++ = k + heightfield->width;
      *i++ = k+1;
      *i++ = k+1 + heightfield->width;

      k++;
    }
    
    k++; // next row
  }

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

  // store vertex texture coordinates (i.e. one triple of floats per vertex)

  GLuint texCoordBufferID;
  glGenBuffers( 1, &texCoordBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, texCoordBufferID );

  glBufferData( GL_ARRAY_BUFFER, nVerts * 2 * sizeof(GLfloat), texCoordBuffer, GL_STATIC_DRAW );

  // attribute 2 = texture coordinates

  glEnableVertexAttribArray( 2 );
  glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  // store faces (i.e. one triple of vertex indices per face)

  GLuint indexBufferID;
  glGenBuffers( 1, &indexBufferID );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferID );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, nFaces * 3 * sizeof(GLuint), indexBuffer, GL_STATIC_DRAW );

  // Clean up

  delete[] vertexBuffer;
  delete[] normalBuffer;
  delete[] texCoordBuffer;
  delete[] indexBuffer;
}



void Terrain::draw( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawUndersideOnly ) 

{
  // Draw textured terrain

  gpu.activate();
  
  gpu.setMat4( "MV", MV );
  gpu.setMat4( "MVP", MVP );
  gpu.setVec3( "lightDir", lightDir );
  gpu.setFloat( "alpha", 1.0 );
  
  const int textureUnitID = 0;
  
  texture->activate( textureUnitID );
  gpu.setInt( "terrainColourSampler", textureUnitID );

  // underside

  const float minZ = -5; // level of underside of terrain box
  
  {
    vec3 pts[4], colours[4];

    vec3 *p = pts;

    *p++ = vec3( 0,                    0,                     minZ );
    *p++ = vec3( 0,                    heightfield->height-1, minZ );
    *p++ = vec3( heightfield->width-1, heightfield->height-1, minZ );
    *p++ = vec3( heightfield->width-1, 0,                     minZ );

    for (int i=0; i<4; i++)
      colours[i] = vec3( BOTTOM_COLOUR );

    segs->drawSegs( GL_TRIANGLE_FAN, pts, colours, 4, MV, MVP, lightDir );
  }

  if (drawUndersideOnly)
    return;

  // Draw using element array

  glBindVertexArray( VAO );

  glDrawElements( GL_TRIANGLES, nFaces*3, GL_UNSIGNED_INT, 0 );

  glBindVertexArray( 0 );

  gpu.deactivate();

  // Draw highlighted quads (for debugging)

  for (int i=0; i<quadsToHighlight.size(); i++) {

    vec3 pts[4], colours[4];
      
    vec3 v = quadsToHighlight[i];
    pts[0] = vec3( v.x, v.y, points[(int)v.x][(int)v.y].z + 0.1 );
    v.x++;
    pts[1] = vec3( v.x, v.y, points[(int)v.x][(int)v.y].z + 0.1 );
    v.y++;
    pts[2] = vec3( v.x, v.y, points[(int)v.x][(int)v.y].z + 0.1 );
    v.x--;
    pts[3] = vec3( v.x, v.y, points[(int)v.x][(int)v.y].z + 0.1 );

    for (int j=0; j<4; j++)
      colours[j] = vec3(1,1,0);

    segs->drawSegs( GL_TRIANGLE_FAN, pts, colours, 4, MV, MVP, lightDir );
  }

  // Draw curtain

  vec3 *pts = new vec3[ 4*(heightfield->width + heightfield->height) ];
  vec3 *colours =  new vec3[ 4*(heightfield->width + heightfield->height) ];

  // ---- draw curtains around terrain ----

  for (unsigned int i=0; i<4*(heightfield->width + heightfield->height); i++)
    colours[i] = vec3( CURTAIN_COLOUR );
  
  vec3 *p = pts;

  // bottom

  int i = 0;
  int j = 0;
  for ( ; i<(int)heightfield->width; i++) {
    *p++ = vec3( i, j, points[i][j].z );
    *p++ = vec3( i, j, minZ );
  }
  i--;
  
  // right

  j++;
  for ( ; j<(int)heightfield->height; j++) {
    *p++ = vec3( i, j, points[i][j].z );
    *p++ = vec3( i, j, minZ );
  }
  j--;
  
  // top

  i--;
  for ( ; i >= 0; i--) {
    *p++ = vec3( i, j, points[i][j].z );
    *p++ = vec3( i, j, minZ );
  }
  i++;
  
  // (xmax,y)

  j--;
  for ( ; j >= 0; j--) {
    *p++ = vec3( i, j, points[i][j].z );
    *p++ = vec3( i, j, minZ );
  }

  segs->drawSegs( GL_TRIANGLE_STRIP, pts, colours, p-pts, MV, MVP, lightDir );

  delete[] pts;
  delete[] colours;
}


// Find the intersection of rayStart + t*rayDir with the terrain.
// planePerp is perpendicular to the vertical plane that embeds this
// ray.


bool Terrain::findIntPoint( vec3 rayStart, vec3 rayDir, vec3 planePerp, vec3 &intPoint, mat4 &M )

{
  // The ray/dir is in the WCS, so we first move it into the OCS of
  // the terrain.

  mat4 Minv = M.inverse();
  
  rayStart = (Minv * vec4( rayStart, 1 )).toVec3();
  rayDir   = (Minv * vec4( rayDir,   0 )).toVec3();

  // Find the first edge of the terrain that is hit by this vertical plane

  float width  = heightfield->width;
  float height = heightfield->height;
  
  vec3 corners[4] = { vec3(0,      0,       0),
                      vec3(width-1,0,       0),
                      vec3(width-1,height-1,0),
                      vec3(0      ,height-1,0) };

  float minDist = MAXFLOAT;
  int minIndex;
  vec3 minPoint;
  
  for (int i=0; i<4; i++) {

    // two adjacent corners

    vec3 &c0 = corners[i];
    vec3 &c1 = corners[(i+1)%4];
    
    // Find t such that (c0 + t(c1-c0))*planePerp = rayStart*planePerp.

    float denom = (c1-c0)*planePerp;
    if (fabs(denom) > 0.001) {  // plane is not parallel to edge
      float t = ((rayStart-c0)*planePerp) / denom;
      if (t >= 0 && t <= 1) {   // plane intersects interior of edge
        float dist = ((c0 + t*(c1-c0)) - rayStart).length();
        if (dist < minDist) {   // closest intersection so far
          minDist = dist;
          minIndex = i;
          minPoint = c0 + t*(c1-c0);
        }
      }
    }
  }

  if (minDist == MAXFLOAT)
    return false;               // no intersection at all

  // Find pixel corners ('pc') that bracket the starting point

  vec3 pc0, pc1;

#define ROUND(x) floor(x+0.5)

  if (minIndex == 0 || minIndex == 2) { // on an edge parallel to x axis
    pc0 = vec3( floor(minPoint.x),   ROUND(minPoint.y), 0 );
    pc1 = vec3( floor(minPoint.x)+1, ROUND(minPoint.y), 0 );
  } else { // on an edge parallel to y axis
    pc0 = vec3( ROUND(minPoint.x), floor(minPoint.y),   0 );
    pc1 = vec3( ROUND(minPoint.x), floor(minPoint.y)+1, 0 );
  }

  // Find the direction in which the vertical plane intersects the
  // terrain.  Point that direction in the direction of rayDir.

  vec3 dir = planePerp ^ vec3(0,0,1);

  if (dir*rayDir < 0)
    dir = -1 * dir;

  // Pick unit x and y increments in the direction 'dir'.

  vec3 xinc( dir.x / fabs(dir.x), 0, 0 );
  vec3 yinc( 0, dir.y / fabs(dir.y), 0 );

  // Name the corners of the boundary pixel ll, lr, ul, and ur.  Make
  // sure that l (= 'lower') has smaller y value than u (= 'up') and
  // that l (= 'left') has smaller x index than r (= 'right').

  vec3 ll, lr, ul, ur;

  switch (minIndex) {
  case 0:                        // y=0 edge
    ll = pc0;
    lr = pc1;
    ul = pc0+yinc;
    ur = pc1+yinc;
    break;
  case 1:                        // x=width edge
    ll = pc0+xinc;
    lr = pc0;
    ul = pc1+xinc;
    ur = pc1;
    break;
  case 2:                        // y=height edge
    ll = pc0+yinc;
    lr = pc1+yinc;
    ul = pc0;
    ur = pc1;
    break;
  case 3:                        // x=0 edge
    ll = pc0;
    lr = pc0+xinc;
    ul = pc1;
    ur = pc1+xinc;
    break;
  }

  // Walk across the pixels of the terrain that intersect the vertical
  // plan.  Stop when a pixel corner goes outside the terrain.

  if (false)
    quadsToHighlight.clear();

  do {

    if (false) {  // show the terrain quad that are traversed in searching for the mouse position on the terrain
      vec3 q( ll.x, ll.y, 0);
      quadsToHighlight.add( q );
    }

    // Set heights of this terrain quad

    ll = points[(int)ll.x][(int)ll.y];
    lr = points[(int)lr.x][(int)lr.y];
    ul = points[(int)ul.x][(int)ul.y];
    ur = points[(int)ur.x][(int)ur.y];

    // Test for intersection of ray with the two terrain triangles
    // above this terrain pixel.

    vec3 p;
    float t;

    if (rayTriangleInt( rayStart, rayDir, ll, lr, ul, p, t )) {
      intPoint = p;
      return true;
    }

    if (rayTriangleInt( rayStart, rayDir, ul, lr, ur, p, t )) {
      intPoint = p;
      return true;
    }

    // Move to the next terrain pixel in direction 'dir' that still
    // brackets the vertical plane.

    vec3 nll, nlr, nul, nur;
    float llDot;

    // Try the x direction

    nll = ll+xinc;
    nlr = lr+xinc;
    nul = ul+xinc;
    nur = ur+xinc;

    llDot = (nll-rayStart) * planePerp;
    if (((nlr-rayStart)*planePerp) * llDot > 0 &&
        ((nul-rayStart)*planePerp) * llDot > 0 &&
        ((nur-rayStart)*planePerp) * llDot > 0) {

      // Try the y direction

      nll = ll+yinc;
      nlr = lr+yinc;
      nul = ul+yinc;
      nur = ur+yinc;
    }

    ll = nll;
    lr = nlr;
    ul = nul;
    ur = nur;

  } while (ll.x >= 0 && ll.x < width && ll.y >= 0 && ll.y < height &&
           lr.x >= 0 && lr.x < width && lr.y >= 0 && lr.y < height &&
           ul.x >= 0 && ul.x < width && ul.y >= 0 && ul.y < height &&
           ur.x >= 0 && ur.x < width && ur.y >= 0 && ur.y < height);

  return false;
}



bool Terrain::rayTriangleInt( vec3 rayStart, vec3 rayDir,
                              vec3 v0, vec3 v1, vec3 v2,
                              vec3 & intPoint, float & intParam )

{
  float param;
  vec3 point, lc;

  vec3 normal = (v1-v0)^(v2-v0);  // not normalized yet!

  // Compute ray/plane intersection

  float dn = rayDir * normal;

  if (fabs(dn) < 0.0001)
    return false;               // ray is parallel to plane

  float dist = v0 * normal;

  param = (dist - rayStart*normal) / dn;
  if (param < 0)
    return false;               // plane is behind starting point

  point = rayStart + param * rayDir;

  // Compute barycentric coords

  float totalArea = ((v1-v0) ^ (v2-v0)) * normal;

  float u = (((v2-v1) ^ (point - v1)) * normal) / totalArea;

  float v = (((v0-v2) ^ (point - v2)) * normal) / totalArea;

  // Reject if outside triangle

  if (u < 0 || v < 0 || u + v > 1)
    return false;

  // Return int point and normal and parameter

  intParam = param / normal.length();
  intPoint = point;

  return true;
}



const char *Terrain::vertShader = R"(

  #version 300 es

  uniform mat4 MVP;
  uniform mat4 MV;

  layout (location = 0) in mediump vec3 vertPosition;
  layout (location = 1) in mediump vec3 vertNormal;
  layout (location = 2) in mediump vec2 vertTexCoords;

  smooth out mediump vec3 normal;
  smooth out mediump vec2 texCoords;

  void main() {

    gl_Position = MVP * vec4( vertPosition, 1.0 );
    normal = vec3( MV * vec4( vertNormal, 0.0 ) );
    texCoords = vertTexCoords;
  }
)";


const char *Terrain::fragShader = R"(

  #version 300 es

  uniform mediump vec3 lightDir;
  uniform mediump float alpha;
  uniform sampler2D terrainColourSampler;

  smooth in mediump vec3 normal;
  smooth in mediump vec2 texCoords;

  out mediump vec4 outputColour;

  void main() {

    mediump float NdotL = dot( normalize(normal), lightDir );

    mediump vec3 colour = texture( terrainColourSampler, texCoords ).rgb;

    if (NdotL < 0.0)
      NdotL = 0.1; // some ambient

    outputColour = vec4( NdotL * colour, alpha );
  }
)";
