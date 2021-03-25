// GPUProgram class


#include "gpuProgram.h"


char* GPUProgram::textFileRead(const char *fileName)

{
  char* text;
    
  if (fileName != NULL) {
    
    FILE *file = fopen( fileName, "rt" );

    if (file == NULL) {

      std::cerr << "File " << fileName << " could not be opened" << std::endl;
      exit(1);

    } else {

      fseek(file, 0, SEEK_END);
      int count = ftell(file);
      rewind(file);
            
      if (count > 0) {
        text = (char*)malloc(sizeof(char) * (count + 1));
        count = fread(text, sizeof(char), count, file);
        text[count] = '\0';
      }

      fclose(file);
    }
  }
  
  return _strdup(text);
}


void GPUProgram::validateShader( GLuint shader, const char* file, const char *shaderName)

{
  const unsigned int BUFFER_SIZE = 512;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  GLsizei length = 0;
    
  glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);

  if (length > 0) {
    std::cout << "Shader " << shader << " (" << shaderName << ") compile log: " << std::endl << buffer << std::endl;
    exit(1);
  }
}


void GPUProgram::validateProgram( const char *shaderName )

{
  const unsigned int BUFFER_SIZE = 512;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  GLsizei length = 0;
    
  memset(buffer, 0, BUFFER_SIZE);

  glGetProgramInfoLog(program_id, BUFFER_SIZE, &length, buffer);

  if (length > 0) {
    std::cout << "Program " << program_id << "(" << shaderName << ") link log: " << buffer << std::endl;
    exit(1);
  }
    
  glValidateProgram( program_id );

  GLint status;
  glGetProgramiv(program_id, GL_VALIDATE_STATUS, &status);

  if (status == GL_FALSE) {
    std::cout << "Error validating program " << program_id << std::endl;
    glGetProgramInfoLog(program_id, BUFFER_SIZE, &length, buffer);
    if (length > 0)
      std::cout << "Program " << program_id << "(" << shaderName << ") link log: " << buffer << std::endl;
    exit(1);
  }
}


void GPUProgram::init( const char *vsTextIn, const char *fsTextIn, const char* shaderName )

{
  char *vsText = strdup(vsTextIn);
  char *fsText = strdup(fsTextIn);

#ifdef MACOS
  // On MacOS, replace "#version 300 es" with "#version 330   " in each shader

  char *p;

  p = strstr( vsText, "#version 300 es" );
  if (p == NULL) {
    cerr << "No \"#version 330 es\" was found in vertex shader." << endl;
    exit(1);

  }

  p[10] = '3'; 
  p[12] = '\n'; 
  p[13] = '\n';
  p[14] = '\n';
  
  p = strstr( fsText, "#version 300 es" );
  if (p == NULL) {
    cerr << "No \"#version 330 es\" was found in fragment shader." << endl;
    exit(1);
  }

  p[10] = '3'; 
  p[12] = '\n'; 
  p[13] = '\n';
  p[14] = '\n';
#endif

  glErrorReport( "before GPUProgram::init" );

  // Vertex shader

  shader_vp = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource( shader_vp, 1, (const char**) &vsText, 0 );
  glCompileShader( shader_vp );
  validateShader( shader_vp, "vertex shader", shaderName );
    
  // Fragment shader

  shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource( shader_fp, 1, (const char **) &fsText, 0 );
  glCompileShader( shader_fp );
  validateShader( shader_fp, "fragment shader", shaderName );
    
  // GLSL program

  program_id = glCreateProgram();
  glAttachShader( program_id, shader_vp );
  glAttachShader( program_id, shader_fp );
  glLinkProgram( program_id );

#ifndef MACOS
  validateProgram( shaderName );
#else
  // MacOS needs a VAO enabled before it can validate the program ... why?
  GLuint dummy;
  glGenVertexArrays( 1, &dummy );
  glBindVertexArray( dummy );
  validateProgram( shaderName );
  glBindVertexArray( 0 );
  glDeleteVertexArrays( 1, &dummy );
#endif

  glUseProgram( program_id );
  glUseProgram( 0 );
  
  glErrorReport( "after GPUProgram::init" );

#ifdef MACOS
  free( vsText );
  free( fsText );
#endif
}


void GPUProgram::initFromFile( const char *vsFile, const char *fsFile, const char* shaderName ) 

{
  char* vsText = textFileRead(vsFile);  
    
  if (vsText == NULL) {
    std::cerr << "Vertex shader file '" << vsFile << "' not found." << std::endl;
    return;
  }
    
  char* fsText = textFileRead(fsFile);
    
  if (fsText == NULL) {
    std::cerr << "Fragment shader file '" << fsFile << "' not found." << std::endl;
    return;
  }

  // Init the shader using these strings
    
  init( vsText, fsText, shaderName );
}
