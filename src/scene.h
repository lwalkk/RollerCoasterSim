// persp.h

#ifndef PERSP_H
#define PERSP_H

#include "headers.h"

#include "gpuProgram.h"
#include "arcball.h"
#include "font.h"
#include "terrain.h"
#include "spline.h"
#include "ctrlPoints.h"
#include "train.h"


#define TRACK_PIECES_PER_SEG  20

#define POST_COLOUR vec3(0.8,0.9,0.5)


class Scene {

  Terrain    *terrain;
  Spline     *spline;
  CtrlPoints *ctrlPoints;
  char       *sceneFile;
  Train      *train;
  Arcball    *arcball;
  GPUProgram *gpu;

  GLFWwindow *window;

  mat4       VCStoCCS;
  float      fovy;

  // user-settable flags

  bool       drawTrack;
  bool       drawCoaster;
  bool       useArcLength;
  bool       showAxes;
  bool       drawUndersideOnly;
  bool       debug;
  bool       pause;
  bool       flag;

  // for mouse picking and dragging:

  bool       arcballActive;
  bool       dragging;
  bool       draggingWasDone;

  vec3       startMousePos;
  vec3       startCtrlPointPos;
  int        selectedCtrlPoint;
  bool       movingSelectedBase;
  static float trainVerts[];

 public:

  Scene( char *sceneFilename, GLFWwindow *w );

  void read( const char *filename );
  bool write();

  void draw( bool useItemTags );

  void mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
  void mouseMovementCallback( GLFWwindow* window, double xpos, double ypos );
  void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
  void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
  
  void mouseClick( vec3 v, int keyModifiers );

  void drawAllTrack( mat4 &MV, mat4 &MVP, vec3 lightDir );

  void update( float elapsedSeconds ) {
    if (ctrlPoints->count() > 1 && !pause)
      train->advance( elapsedSeconds );
    terrain->setTime(elapsedSeconds);
  }

  void getMouseRay( int mouseX, int mouseY, vec3 &rayStart, vec3 &rayDir );

  void readView();
  void writeView();
};


#endif
