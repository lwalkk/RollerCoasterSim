// font.cpp
//
// From https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
//
// You must install FreeType: https://www.freetype.org/download.html
//
// The <ft2build.h> should be on your 'include' path


#ifndef HAVE_FREETYPE

#include "headers.h"
void initFont( const char *ttf_file, int height_in_pixels ) {}
void render_text( const char *text, int pixOriginX, int pixOriginY, GLFWwindow* window ) {}

#else

#include "headers.h"
#include "gpuProgram.h"
#include <cstring>

#include <ft2build.h>           // requires the freetype library.  On Linux: "sudo apt-get install libfreetype6-dev"
#include FT_FREETYPE_H


static FT_Library ft;
static FT_Face face;
static GLuint vao, vbo, tex;
static GPUProgram *gpu;


char *vertexShader = "\n\
#version 300 es\n\
\n\
layout (location = 0) in vec4 coord;\n\
smooth out mediump vec2 texcoord;\n\
\n\
void main(void) {\n\
  gl_Position = vec4(coord.xy, 0, 1);\n\
  texcoord = coord.zw;\n\
}";

char *fragmentShader = "\n\
#version 300 es\n\
\n\
uniform sampler2D tex;\n\
uniform mediump vec4 colour;\n\
smooth in mediump vec2 texcoord;\n\
out mediump vec4 fragColour;\n\
\n\
void main(void) {\n\
  mediump float t = texture(tex, texcoord).r;\n\
  fragColour = vec4(1,1,1,t) * colour ;\n\
}";




void initFont( const char *ttf_file, int height_in_pixels )

{
  if(FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Could not init freetype library\n");
    exit(1);
  }

  if(FT_New_Face(ft, ttf_file, 0, &face)) {
    fprintf(stderr, "Could not open font\n");
    exit(1);
  }

  FT_Set_Pixel_Sizes(face, 0, height_in_pixels);

  glGenTextures( 1, &tex );
  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );

  gpu = new GPUProgram();
  gpu->init( vertexShader, fragmentShader, "in font.cpp" );
}



// Print a string at (x,y) on the display
//
// x,y is the start position in [-1,1]x[-1,1]
// sx,sy are the dimensions of a pixel in the [-1,1]x[-1,1] window coordinate system


void render_text( const char *text, int pixOriginX, int pixOriginY, GLFWwindow* window )

{
  // Get size of a pixel in world coordinates [-1,1]x[-1,1]

  int width, height;
  glfwGetFramebufferSize( window, &width, &height );

  float sx = 2.0 / (float) width;
  float sy = 2.0 / (float) height;

  // Postion the first character in world coordinates

  float x = -1 + pixOriginX * sx;
  float y = -1 + pixOriginY * sy;

  // Code common to all character textures

  if (tex != 1)
    cout << "Font using texture " << tex << endl;

  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, tex );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  // GPU init

  gpu->activate();
  gpu->setInt( "tex", 1 ); // texture is managed by texture unit 1 (= GL_TEXTURE1 above)
  gpu->setVec4( "colour", vec4(0,0,0,1) ); // character colour

  // Code common to all VBO init

  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  
  glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( 0 );

  // Each character is drawn with one texture that provides the opacity

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  FT_GlyphSlot g = face->glyph;

  for (const char *p=text; *p != '\0'; p++) {
    
    if ( FT_Load_Char(face, *p, FT_LOAD_RENDER) )
      continue; // no texture available for this character
 
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer );
 
    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;
 
    GLfloat box[4][4] = {
      {x2,     -y2    , 0, 0},
      {x2 + w, -y2    , 1, 0},
      {x2,     -y2 - h, 0, 1},
      {x2 + w, -y2 - h, 1, 1},
    };

    glBufferData( GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    x += (g->advance.x/64) * sx;
    y += (g->advance.y/64) * sy;
  }

  // Disable everything

  glDisable( GL_BLEND );
  glDisableVertexAttribArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  gpu->deactivate();

  glBindVertexArray( 0 );
}

#endif
