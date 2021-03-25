// The standard headers included by all files


#ifndef HEADERS_H
#define HEADERS_H

#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>

#include <sys/timeb.h>  // includes ftime (to return current time)

#ifdef LINUX
  #include <unistd.h>           // includes usleep (to sleep for some time)
  #include <values.h>           // includes MAX_FLOAT
  #define sprintf_s sprintf
  #define _strdup strdup
  #define sscanf_s sscanf
#endif

#ifdef _WIN32
  //#include <typeinfo>
  #define M_PI 3.14159
  #define MAXFLOAT FLT_MAX
  //#define rint(x) floor((x)+0.5)
  #pragma warning(disable : 4244 4305 4996 4838)
  #define chdir _chdir
  #include <direct.h>
#endif

#ifdef MACOS
  #include <unistd.h>           // includes usleep (to sleep for some time)
  #define _strdup strdup
#endif

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;

#include <cmath>

#include "linalg.h"

#define randIn01() (rand() / (float) RAND_MAX)   // random number in [0,1]

#endif
