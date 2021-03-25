/* texture.h
 */


#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"

class Texture {

  GLubyte *texmap; 

  void registerWithOpenGL();
  void loadTexture( string filename );

 public:

  string name;
  GLuint textureID;
  unsigned int width, height;
  bool hasAlpha;

  static bool useMipMaps;

  Texture() {}

  Texture( string basePath, string filename ) {
    name = filename;
    loadTexture( basePath + string("/") + filename );
    registerWithOpenGL();
  }

  void activate( int textureUnit ) {
    glActiveTexture( GL_TEXTURE0 + textureUnit );
    glBindTexture( GL_TEXTURE_2D, textureID );
    if (hasAlpha) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else
      glDisable(GL_BLEND);
  }

  vec3 texel( int i, int j, float &alpha );
};


#endif
