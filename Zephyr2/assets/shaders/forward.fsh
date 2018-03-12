#version 330 core
in vec2 oTexC;
in vec3 oNormal;
in vec3 oWorldPos;
uniform sampler2D iTexture;
uniform float iSmoothness;
uniform vec3 iAmbient;
uniform vec3 iCameraPos; 
uniform vec3 iDLight;
uniform vec3 iDLightFacing;
uniform mat4 iMM;

void main()
{
	vec4 texColor = texture(iTexture, vec2(oTexC.x, oTexC.y));
	
	vec3 lightDir = normalize(-iDLightFacing);
	vec3 eyeDir = normalize(iCameraPos - oWorldPos);
	vec3 normal = normalize(oNormal);
	
	float diffuseC = max(0.0, dot(normal, lightDir));
    float diffuse = diffuseC * (1.0 - iSmoothness);
	
	float spec = 0.0;
	if(diffuseC > 0)
	{
		spec = pow(max(dot(reflect(-lightDir, normal), eyeDir),0.0), 50) * iSmoothness;
	}	
	
	vec3 allLights = (iDLight * diffuse) + (iDLight * spec) + iAmbient;
	
	gl_FragColor.rgb = texColor.rgb * allLights;
	gl_FragColor.a = texColor.a;

}