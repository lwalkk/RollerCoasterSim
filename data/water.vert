#version 300 es

uniform mat4 MVP;
uniform mat4 MV;
uniform sampler2D normalSampler;

layout (location = 0) in mediump vec3 vertPosition;
layout (location = 1) in mediump vec3 vertNormal;
layout (location = 2) in mediump vec2 vertTexCoords;

smooth out mediump vec3 normal_vcs;
smooth out mediump vec3 position_vcs;
smooth out mediump vec2 texCoords;

void main()
{
    gl_Position = MVP * vec4(vertPosition, 1.0);
	texCoords = vertTexCoords;
    mediump vec3 texNormal = texture(normalSampler, texCoords).rgb;
    normal_vcs = vec3( MV * vec4(texNormal, 0.0));
    position_vcs = vec3(MV * vec4(vertPosition, 1.0));
}