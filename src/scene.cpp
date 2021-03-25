// persp.cpp


#include "headers.h"
#include "scene.h"
#include "font.h"
#include "main.h"

#include <strstream>
#include <fstream>
#include <iomanip>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIGHT_DIR 1,1,3

Scene::Scene( char *sceneFilename, GLFWwindow *w )

{
  window = w;

  glfwSetWindowUserPointer( window, this );
  
  // Key handler

  auto static_keyCallback = [](GLFWwindow* ww, int key, int scancode, int action, int mods ) {
    static_cast<Scene*>(glfwGetWindowUserPointer(ww))->keyCallback( ww, key, scancode, action, mods );
  };
 
  glfwSetKeyCallback( window, static_keyCallback );

  // Mouse callbacks

  auto static_mouseButtonCallback = [](GLFWwindow* ww, int button, int action, int keyModifiers ) {
    static_cast<Scene*>(glfwGetWindowUserPointer(ww))->mouseButtonCallback( ww, button, action, keyModifiers );
  };
 
  glfwSetMouseButtonCallback( window, static_mouseButtonCallback );

  auto static_mouseScrollCallback = [](GLFWwindow* ww, double xoffset, double yoffset ) {
    static_cast<Scene*>(glfwGetWindowUserPointer(ww))->mouseScrollCallback( ww, xoffset, yoffset );
  };
 
  glfwSetScrollCallback( window, static_mouseScrollCallback );

  // Set up arcball interface

  arcball = new Arcball( window );

  readView();  // change eye position from default if 'view.txt' exists

  // Set up GPU program
  
  gpu = new GPUProgram( "data/shader.vert", "data/shader.frag", "shader files for scene.cpp" );

  // Set up scene
   
  spline     = new Spline();
  ctrlPoints = new CtrlPoints( spline, window );
  train      = new Train( spline );

  read( sceneFilename );

  // Miscellaneous stuff

  pause = false;
  dragging = false;
  arcballActive = false;
  drawTrack = false;
  drawCoaster = true;
  drawUndersideOnly = false;
  useArcLength = false;
  showAxes = false;
  debug = false;
  flag = false;
}


void Scene::draw( bool useItemTags )

{
  glClearColor( 0,0,0, 0 );

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );

  // model-view transform (i.e. OCS-to-VCS)

  float diag = sqrt( terrain->texture->width*terrain->texture->width + terrain->texture->height*terrain->texture->height );
  
  VCStoCCS = perspective( fovy, 
                          windowWidth/(float)windowHeight, 
                          MAX(0.1,arcball->distToCentre - 1.2*diag),
                          arcball->distToCentre + 1.2*diag );

  mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );
  mat4 V = arcball->V;

  mat4 MV = V * M;
  mat4 MVP = VCStoCCS * MV;
  
  vec3 lightDir = vec3( LIGHT_DIR ).normalize();

  // Draw control points

  ctrlPoints->draw( drawTrack, MV, MVP, lightDir, POST_COLOUR );

  if (ctrlPoints->count() > 1) {
    if (drawTrack)
      drawAllTrack( MV, MVP, lightDir );
    else { // Draw spline
      if (useArcLength)
        spline->drawWithArcLength( MV, MVP, lightDir, debug );
      else
        spline->draw( MV, MVP, lightDir, debug );
    }
  }

  // Draw heightfield

  gpu->activate();

  gpu->setMat4( "MV",  MV );
  gpu->setMat4( "MVP", MVP );

  gpu->setVec3( "lightDir", lightDir );

  terrain->draw( MV, MVP, lightDir, drawUndersideOnly );

  gpu->deactivate();

  // Draw train

  if (ctrlPoints->count() > 1 && drawCoaster)
    train->draw( MV, MVP, lightDir, flag );

  // Now the axes
    
  if (showAxes) {
    MVP = VCStoCCS * V * scale(10,10,10); // no object transformation, since axes are already at origin and aligned.
    axes->draw( MVP );
  }

  // Draw status message

  ostrstream message;
  message << "using " << spline->name() << "        speed " << std::setprecision(2) << train->getSpeed() << '\0';
  render_text( message.str(), 10, 10, window );

  // Done
  
  glfwSwapBuffers( window );
}



// Key callback


void Scene::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )

{
  if (action == GLFW_PRESS)
    switch (key) {

    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose( window, GL_TRUE );
      break;

    case 'D':
      debug = !debug;           // enable/disable debugging
      break;

    case 'T':
      drawTrack = !drawTrack;   // enable/disable track drawing
      break;

    case 'C':
      drawCoaster = !drawCoaster; // enable/disable coaster drawing
      break;

    case 'A':
      useArcLength = !useArcLength; // enable/disable arc-length drawing
      break;

    case 'U':
      drawUndersideOnly = !drawUndersideOnly; // enable/disable drawing of terrain
      break;

    case 'P':
      pause = !pause;
      break;

    case 'F':
      flag = !flag;  // general purpose
      break;

    case 'S':                   // store the scene
      if (write())
        cout << "Scene stored in '" << sceneFile << "'." << endl;
      else
        cerr << "FAILED to store scene in '" << sceneFile << "'." << endl;
      break;

    case 'M':                   // change the change-of-basis matrix
      spline->nextCOB();
      break;

    case '+':
    case '=':
      train->accelerate();
      break;

    case '-':
    case '_':
      train->brake();
      break;

    case 'R':
      readView();
      break;

    case 'W':
      writeView();
      break;

    case 'X':
      showAxes = !showAxes;
      break;

    case '/':  // = ?
      cout << "Click to add a control point." << endl
           << "Ctrl-click to delete a control point." << endl
           << "Move a control point by dragging its base." << endl
           << "Change a control point's height by dragging its top." << endl
           << endl
	   << "-/+ change train speed" << endl
           << "a - toggle arc length parameterization" << endl
           << "c - toggle coaster drawing" << endl
           << "d - toggle debug mode (shows local coordinate frame on track)" << endl
           << "f - toggle flag (useful for debugging)" << endl
           << "m - cycle through CoB matrices" << endl
           << "p - toggle pause" << endl
           << "r - read initial view" << endl
           << "s - store scene in '" << sceneFile << "'" << endl
           << "t - toggle track drawing" << endl
           << "u - toggle underside of terrain" << endl
           << "w - write initial view (for use on next startup)" << endl
           << "x - toggle world axes" << endl
        ;

        break;
    }
}




void Scene::readView()

{
  ifstream in( "data/view.txt" );

  if (!in)
    return;

  in >> arcball->V;
  in >> arcball->distToCentre;

  float angle;
  in >> angle;
  fovy = angle/180.0*M_PI;
}


void Scene::writeView()

{
  ofstream out( "data/view.txt" );

  out << arcball->V << endl;
  out << arcball->distToCentre << endl;
  out << fovy*180/M_PI << endl;
}




// Return rayStart and rayDir for the ray in the WCS from the
// viewpoint through the current mouse position (mouseX,mouseY).

void Scene::getMouseRay( int mouseX, int mouseY, vec3 &rayStart, vec3 &rayDir )

{
  vec4 ccsMouse( mouseX/(float)windowWidth*2-1, -1 * (mouseY/(float)windowHeight*2-1), 0, 1 );  // [-1,1]x[-1,1]x0x1
  vec3 wcsMouse = ((VCStoCCS * arcball->V).inverse() * ccsMouse).toVec3();
  
  rayStart = arcball->eyePosition();
  rayDir   = (wcsMouse - rayStart).normalize();
}




void Scene::mouseButtonCallback( GLFWwindow* window, int button, int action, int keyModifiers )

{
  // Get mouse position
  
  double xpos, ypos;
  glfwGetCursorPos( window, &xpos, &ypos );

  if (action == GLFW_PRESS) {

    // Handle mouse press

    // Start tracking the mouse

    auto static_mouseMovementCallback = [](GLFWwindow* window, double xpos, double ypos ) {
      static_cast<Scene*>(glfwGetWindowUserPointer(window))->mouseMovementCallback( window, xpos, ypos );
    };
 
    glfwSetCursorPosCallback( window, static_mouseMovementCallback );

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {

      // 'arcball' handles right mouse event

      arcballActive = true;

      arcball->mousePress( button, xpos, ypos );

    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {

      // 'scene' handles left mouse events

      static vec3 lastMousePos;

      lastMousePos.x = xpos;
      lastMousePos.y = ypos;

      draggingWasDone = false;

      vec3 start,dir;
      getMouseRay( xpos, ypos, start, dir ); // sets 'start' and 'dir'

      mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );

      int hitID = ctrlPoints->findSelectedPoint( start, dir, M );

      if (hitID >= 0) {

        // mouse has picked a control point handle

        startMousePos = vec3( xpos, ypos, 0 );

        dragging = true;

        selectedCtrlPoint = hitID / 2;
        movingSelectedBase = ((hitID % 2) == 0); // base names are even; top names are odd
        startCtrlPointPos = ctrlPoints->points[selectedCtrlPoint];
      }
    }

  } else { 

    // handle mouse release

    if (!draggingWasDone && button == GLFW_MOUSE_BUTTON_LEFT) 
      mouseClick( vec3( xpos, ypos, 0 ), keyModifiers );

    glfwSetCursorPosCallback( window, NULL );
    dragging = false;
    arcballActive = false;
  }
}




// Mouse callbacks

void Scene::mouseMovementCallback( GLFWwindow* window, double xpos, double ypos )

{
  if (arcballActive) {
    arcball->mouseDrag( xpos, ypos );
    return;
  }

  if (!dragging)
    return;

  draggingWasDone = true;

  // Find mouse in world

  vec3 start,dir;
  getMouseRay( xpos, ypos, start, dir ); // sets 'start' and 'dir'

  vec3 updir = arcball->upDirection();
  vec3 n     = (dir ^ updir).normalize();

  mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );

  // Perform the action

  if (movingSelectedBase) {

    // Moving the base along the terrain

    vec3 intPoint;
    bool found = terrain->findIntPoint( start, dir, n, intPoint, M );

    if (found)
      ctrlPoints->moveBase( selectedCtrlPoint, intPoint );
    
  } else {

    // Moving the top up or down

    // Find the plane through eye and mouseInWorld

    vec3 n = ((dir ^ updir) ^ dir).normalize();
    float d = n * start;

    // Find where the vertical line from the base upward intersects this plane
    //
    // Solve for t in n*(base + t*vertical) = d

    vec3 base = (M * vec4( ctrlPoints->bases[ selectedCtrlPoint ], 1 )).toVec3();
    vec3 vertical(0,0,1);

    if (fabs(n*vertical) < 0.001)
      return;

    float t = (d - n*base) / (n*vertical);

    if (t < 1)
      return;

    ctrlPoints->setHeight( selectedCtrlPoint, t );
  }
}



void Scene::mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset )

{
  arcball->mouseScroll( xoffset, yoffset ); // tell arcball about this
}



void Scene::mouseClick( vec3 mousePosition, int keyModifiers )

{
  // Find ray from eye through mouse

  mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );

  vec3 start,dir;
  getMouseRay( mousePosition.x, mousePosition.y, start, dir ); // sets 'start' and 'dir'

  vec3 updir = arcball->upDirection();
  vec3 n     = (dir ^ updir).normalize();

  if (keyModifiers & GLFW_MOD_CONTROL) {

    // CTRL is held down.  Delete the control point under the mouse

    int hitID = ctrlPoints->findSelectedPoint( start, dir, M );

    if (hitID >= 0)
      ctrlPoints->deletePoint( hitID/2 ); // IDs i and i+1 are for the bottom/top of a post.  Post ID is i/2.

  } else {

    // CTRL is not held down.  Insert a new control point.

    vec3 intPoint;

    if (terrain->findIntPoint( start, dir, n, intPoint, M ))
      ctrlPoints->addPoint( intPoint );
  }
}



// read a scene file


void Scene::read( const char *filename )

{
  sceneFile = strdup( filename );

  // Find directory of this scene file

  char *basePath = strdup( sceneFile );
  char *p = strrchr( basePath, '/' );
  if (p != NULL)
    *p = '\0';
  else {
    char* p = strrchr(basePath, '\\');
    if (p != NULL)
      *p = '\0';
    else
      basePath[0] = '\0';
  }

  // Open file
  
  ifstream in( filename );

  if (!in) {
    cerr << "Could not open file '" << filename << "'." << endl;
    exit(1);
  }

  string cmd;
  in >> cmd;
  while (in) {

    if (cmd == "terrain") {

      string heightFile, textureFile;
      in >> heightFile >> textureFile;

      terrain = new Terrain( string(basePath), heightFile, textureFile );
      in >> cmd;

    } else if (cmd == "points") {

      ctrlPoints->clear();

      in >> cmd;

      while (in && (isdigit(cmd.c_str()[0]) || cmd.c_str()[0] == '-' || cmd.c_str()[0] == '.')) {
        float y, z, h;
        in >> y >> z >> h;
        ctrlPoints->addPointWithHeight( vec3( atof(cmd.c_str()), y, z ), h );
        in >> cmd;

      }
    }
  }

  free( basePath ); 
}


// Write to the scenefile


bool Scene::write()

{
  ofstream out( sceneFile );

  if (!out)
    return false;

  out << "terrain" << endl;
  out << "  " << terrain->heightfield->name << endl;
  out << "  " << terrain->texture->name << endl;
  out << endl;
  out << "points" << endl;
  for (int i=0; i<ctrlPoints->bases.size(); i++)
    out << "  " << ctrlPoints->bases[i] << " " << ctrlPoints->points[i].z - ctrlPoints->bases[i].z << endl;

  return true;
}


// Draw the track


#define DIST_BETWEEN_TIES 15.0
#define NUM_SEGMENTS_BETWEEN_TIES 4


void Scene::drawAllTrack( mat4 &MV, mat4 &MVP, vec3 lightDir )

{
    // loop and get each local coordinate system according to arc length parameterization

    float totalLength = spline->totalArcLength();
    float size = (spline->data).size();

    float ss_inc = totalLength / (float)(size * DIVS_PER_SEG);

    vec3 o, x, y, z;
    vec3 o1, x1, y1, z1;
    vec3 o2, x2, y2, z2;

    float t0 = spline->paramAtArcLength(0);
    spline->findLocalSystem(t0, o1, x1, y1, y2);

    Cube cube;

    mat4 V = arcball->V;
    mat4 modelView;
    mat4 modelViewProjection;
    mat4 M_base = translate(-1 * (int)(terrain->texture->width) / 2, -1 * (int)(terrain->texture->height) / 2, 0);
    float scaleFactor = ss_inc;


    for (float ss = ss_inc; ss < totalLength - ss_inc; ss += ss_inc)
    {
        float t = spline->paramAtArcLength(ss);
        spline->findLocalSystem(t, o, x, y, z);

        // now find what the translation matricesc need to be


        //float theta = vec3(1, 0, 0) * z2;
        //vec3 axis = vec3(1, 0, 0) ^ z2;
       // mat4 M = translate(o) * M_base * rotate(theta, axis) * scale(5, 1, 1);

        mat4 M;
        M.rows[0] = vec4(x.x, y.x, z.x, o.x);
        M.rows[1] = vec4(x.y, y.y, z.y, o.y);
        M.rows[2] = vec4(x.z, y.z, z.z, o.z);
        M.rows[3] = vec4(0, 0, 0, 1);
      
       
        modelView = V * M_base * M  * scale(0.5, 0.5, 6);
        modelViewProjection = VCStoCCS * modelView;

        cube.Draw(modelView, modelViewProjection, lightDir, vec3(1, 1, 1));


        o1 = o2;
        x1 = x2;
        y1 = y2;
        z1 = z2;

    }
}
