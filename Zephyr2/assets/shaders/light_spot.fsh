#version 330 core
in vec2 uv;
out vec3 color;
uniform sampler2D fColor;
uniform sampler2D fPosition;
uniform sampler2D fNormal;
uniform sampler2D fDepth;
uniform vec3 cameraPos;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightFacing;
uniform float lightRange;
uniform float lightIntensity;
uniform float lightAngle;
void main()
{
	float smoothness = texture(fColor, uv).a;
	vec3 image = texture( fColor, uv).rgb;
    vec3 position = texture( fPosition, uv).rgb;
    vec3 normal = texture( fNormal, uv).rgb;
	
	vec3 lightColorFac = lightColor * lightIntensity;
	vec3 lightVec = lightPos - position;
	vec3 lightDir = normalize(lightVec);
	float lightDist = length(lightVec);
	normal = normalize(normal);
	
	vec3 eyeDir = normalize(cameraPos - position);
	vec3 coneDir = normalize(lightFacing);
	vec3 rayDir = -lightDir;
	float lightSurfAngle = acos(dot(rayDir, coneDir));
	
	float attn;
	if(lightSurfAngle > lightAngle)
	{
		attn = 0.0;
	}
	else
	{
		attn = pow(clamp(1.0 - lightDist/lightRange, 0.0, 1.0), 2);
	}
	
	
	float diffuseC = max(0.0, dot(normal, lightDir)); 
	float diffuse = diffuseC * attn * (1.0 - smoothness);
	
	float spec = 0.0;
	if(diffuseC > 0)
	{
		spec = pow(max(dot(reflect(-lightDir, normal), eyeDir),0.0), 50) * attn * smoothness;
	}	
	
	
	color.rgb = (diffuse * lightColorFac) + (spec * lightColorFac);
}