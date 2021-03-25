// Rollercoaster


#include "headers.h"
#include "scene.h"
#include "font.h"
#include "main.h"

// window dimensions

int windowWidth  = 800;
int windowHeight = 600;


// The whole scene

Scene *scene;

Sphere   *sphere;
Cylinder *cylinder;
Axes     *axes;
Segs     *segs;
Cube* cube;

// Error callback

void errorCallback( int error, const char* description )

{
  cerr << "Error " << error << ": " << description << endl;
  exit(1);
}


// Callbacks for when window size changes

void windowReshapeCallback( GLFWwindow* window, int width, int height )

{
  windowWidth = width;
  windowHeight = height;
}

void framebufferReshapeCallback( GLFWwindow* window, int width, int height )

{
  glViewport( 0, 0, width, height );
}


// Main program

int main( int argc, char **argv )

{
  // Get scene file name

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " scene_name" << endl;
    exit(1);
  }

  char *sceneFilename = argv[1];

  std::cout << sceneFilename << std::endl;

  chdir( ".." );

  // Initialize the window

  glfwSetErrorCallback( errorCallback );
  
  GLFWwindow* window;

  if (!glfwInit())
    return 1;
  
#ifdef MACOS
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#else
  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
#endif

  window = glfwCreateWindow( windowWidth, windowHeight, "Rollercoaster", NULL, NULL);
  
  if (!window) {
    glfwTerminate();
    return 1;
  }

#if 0
  glfwSetWindowPos(window, 2000, 100);
#endif
  
  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 );
  gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );

  glfwSetWindowSizeCallback( window, windowReshapeCallback );
  glfwSetFramebufferSizeCallback( window, framebufferReshapeCallback );

  // Fonts

  initFont( "./FreeSans.ttf", 20 );
  
  // Set up the scene (and event handlers for mouse and keyboard)

  scene = new Scene( sceneFilename, window );

  // Some basic objects

  sphere   = new Sphere(4);
  cylinder = new Cylinder(200);
  axes     = new Axes();
  segs     = new Segs();
  cube = new Cube();
  
  // Main loop

  struct timeb prevTime, thisTime; // record the last rendering time
  ftime( &prevTime );

  while (!glfwWindowShouldClose( window )) {

    // Update the world state

    ftime( &thisTime );
    float elapsedSeconds = (thisTime.time + thisTime.millitm / 1000.0) - (prevTime.time + prevTime.millitm / 1000.0);
    prevTime = thisTime;

    scene->update( elapsedSeconds );
    scene->draw( false ); // false = draw normally

    glfwPollEvents();
  }

  // Clean up

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}
