#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNorm;
layout (location = 2) in vec2 iTexC;
out vec2 oTexC;
out vec3 oNormal;
out vec3 oWorldPos;
uniform mat4 iModelViewProjectionMatrix;
uniform mat4 iModelMatrix;
void main()
{
   gl_Position = iModelViewProjectionMatrix * vec4(iPos.x, iPos.y, iPos.z, 1.0);
   oTexC = iTexC;
   oNormal = (iModelMatrix * vec4(iNorm, 0.0)).xyz;
   oWorldPos = (iModelMatrix * vec4(iPos, 1.0)).xyz;
}