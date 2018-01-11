#version 330 core
layout (location = 0) in vec3 iPos;
uniform mat4 iModelViewProjectionMatrix;
void main()
{
   gl_Position = iModelViewProjectionMatrix * vec4(iPos, 1.0);
}