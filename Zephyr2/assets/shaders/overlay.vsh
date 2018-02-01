#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iTexC;
out vec2 oTexC;
uniform mat4 iMVPM;
void main()
{
	gl_Position = iMVPM * vec4(iPos.x, iPos.y, iPos.z, 1.0);
	oTexC = iTexC;
}