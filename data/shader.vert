// vertex shader

#version 300 es

uniform mat4 MVP;		// OCS-to-CCS
uniform mat4 MV;		// OCS-to-VCS

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;

flat out mediump vec3 colour;
smooth out mediump vec3 normal;


void main()

{
  // calc vertex position in CCS

  gl_Position = MVP * vec4( vertPosition, 1.0f );

  // assign (r,g,b) colour equal to (x,y,z) position.  But the (x,y,z)
  // components have to be transformed from [-1,1] to [0,1].

  colour = 0.5 * (vertPosition + vec3(1.0,1.0,1.0));

  // calc normal in VCS
  //
  // To do this, apply the non-translational parts of MV to the model
  // normal.  The 0.0 as the fourth component of the normal ensures
  // that no translational component is added.

  normal = vec3( MV * vec4( vertNormal, 0.0 ) );
}
