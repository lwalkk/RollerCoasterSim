// fragment shader

#version 300 es

uniform mediump vec3 lightDir; // direction *toward* light in VCS

flat in mediump vec3 colour;
smooth in mediump vec3 normal;

out mediump vec4 outputColour;


void main()

{
  // attenuate colour by cosine of angle between fragment normal and
  // light direction.  But ... if dot product is negative, light is
  // behind fragment and fragment is black (or can appear grey if some
  // "ambient" light is added).

  mediump float NdotL = dot( normalize(normal), lightDir );

  if (NdotL < 0.0)
    NdotL = 0.0;

  mediump vec3 diffuseColour = NdotL * colour;

  //outputColour = vec4( colour, 1.0 );

  //outputColour = vec4( diffuseColour, 1.0 );

  mediump float ambient = 0.15;
  outputColour = vec4( (1.0-ambient) * diffuseColour + ambient * colour, 1.0 );
}
