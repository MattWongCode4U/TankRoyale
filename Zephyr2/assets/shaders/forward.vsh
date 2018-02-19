#version 330 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNorm;
layout (location = 2) in vec2 iTexC;
out vec2 oTexC;
out vec3 oNormal;
out vec3 oWorldPos;
uniform mat4 iMVM;
uniform mat4 iPM;
uniform mat4 iMM;
uniform vec4 iOffsets;
uniform bool iAnimated;

void main()
{
	//TODO billboard handling... it's coming later because it's more complicated than I thought

	gl_Position = (iPM * iMVM) * vec4(iPos.x, iPos.y, iPos.z, 1.0);
	
	if(iAnimated)
		oTexC = iOffsets.xy + (vec2( iTexC.x, 1.0f - iTexC.y) * iOffsets.zw);		
	else
		oTexC = vec2(iTexC.x, 1.0-iTexC.y);
		
	oNormal = (iMM * vec4(iNorm, 0.0)).xyz;
	oWorldPos = (iMM * vec4(iPos, 1.0)).xyz;
}