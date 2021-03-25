// texture.cpp


#include "texture.h"
#include "lodepng.h"


bool Texture::useMipMaps = false;


// Register the current texture with OpenGL, assigning it a textureID.


void Texture::registerWithOpenGL( )

{
  // Register it with OpenGL

  glGenTextures( 1, &textureID );
  glBindTexture( GL_TEXTURE_2D, textureID );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  glTexImage2D( GL_TEXTURE_2D, 0, (hasAlpha ? GL_RGBA : GL_RGB), width, height, 0,
                (hasAlpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, texmap );

  glGenerateMipmap( GL_TEXTURE_2D );
}



void Texture::loadTexture( string filename )

{
  // Read image
  
  std::vector<unsigned char> image;

  unsigned error = lodepng::decode( image, width, height, filename.c_str() );

  if (error)
    std::cerr << "Error loading '" << filename << "': " << lodepng_error_text(error) << std::endl;

  // Copy image to our own texmap
  
  texmap = new GLubyte[ width * height * 4 ];

  GLubyte *p = texmap;
  for (unsigned int i=0; i<width*height*4; i++)
    *p++ = image[i];

  hasAlpha = true;
}





// Find the texel at x,y for x,y in [0,width-1]x[0,height-1]

vec3 Texture::texel( int x, int y, float &alpha )

{
  if (x<0) x = 0;
  if (x>(int)width-1) x = width-1;
  if (y<0) y = 0;
  if (y>(int)height-1) y = height-1;

  unsigned char *p;
  vec3 colour;

  p = texmap + (hasAlpha ? 4 : 3) * (y*width + x);

  colour.x = (*p++)/255.0f;
  colour.y = (*p++)/255.0f;
  colour.z = (*p++)/255.0f;

  if (hasAlpha)
    alpha = (*p++)/255.0f;
  else
    alpha = 1;

  return colour;
}
