 #version 300 es

  uniform mediump vec3 lightDir;
  uniform mediump float alpha;
  uniform sampler2D terrainColourSampler;
  uniform sampler2D distortionSampler; 

  uniform mediump float time;

  smooth in mediump vec3 normal_vcs;
  smooth in mediump vec3 position_vcs;
  smooth in mediump vec2 texCoords;
 

  out mediump vec4 outputColour;
 

  void main() 
  {
    // normals and direction to viewpoint
    mediump vec3 N = normalize(normal_vcs);
    mediump float NdotL = dot(N, lightDir);

    mediump vec3 V = normalize( -1.0 * position_vcs);
    mediump vec3 R = (2.0 * NdotL) * N - lightDir;
    mediump float RdotV = dot(R, V);
 
    // determine distortion effects 
    mediump vec2 distortion = texture(distortionSampler, texCoords).rg;
    mediump float period = fract(time / 4.0 );

    // ambient lighting
    mediump vec3 Ia = vec3(0.2, 0.2, 0.2);

    // surface color 
    mediump vec3 kd = texture( terrainColourSampler, texCoords - period * distortion).rgb;

    //specular coefficients
    mediump vec3 ks = vec3(0.7, 0.7, 0.7);
    mediump float shininess = 100.0;

    mediump vec3 Iout = Ia;

    Iout += NdotL * kd; 
    
    Iout += pow(RdotV, shininess) * ks;




    outputColour = vec4( Iout, alpha );
  }
