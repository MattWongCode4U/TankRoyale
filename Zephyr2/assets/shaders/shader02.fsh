#version 330 core
in vec2 uv;
out vec4 color;
uniform sampler2D fColor;
uniform sampler2D fPosition;
uniform sampler2D fNormal;
uniform sampler2D fDepth;
uniform sampler2D sDepth;
uniform vec3 clearColor;
uniform vec3 ambientLight;
uniform vec3 cameraPos; 
uniform vec3 dLightColor;
uniform vec3 dLightFacing;
uniform mat4 biasMVP;
void main()
{
	float smoothness = texture(fColor, uv).a;
	vec3 position = texture( fPosition, uv).rgb;
	vec3 normal = texture( fNormal, uv).rgb; 	
	
	//run shadow calculations
	vec4 shadowCoord = biasMVP * vec4(position,1);
	
	float shadowValue =  texture( sDepth, shadowCoord.xy ).r ;
	
	float bias = 0.025;
	float visibility = 1.0;
	if ( shadowValue  <  shadowCoord.z-bias)
	{
		visibility = 0.0;
	}
	
	//calculate diffuse and specular
	vec3 lightDir = normalize(-dLightFacing);
	vec3 eyeDir = normalize(cameraPos - position);
	vec3 nNormal = normalize(normal);
	
	float diffuseC = max(0.0, dot(normal, lightDir));
    float diffuse = diffuseC * (1.0 - smoothness);
	
	float spec = 0.0;
	if(diffuseC > 0)
	{
		spec = pow(max(dot(reflect(-lightDir, normal), eyeDir),0.0), 50) * smoothness;
	}	
	
	vec3 sDirectionalLight = ((dLightColor * diffuse) + (dLightColor * spec)) * visibility;
	
	//shader-based clear color necessitated by the way we're doing deferred rendering
	float depth = texture(fDepth, uv).r;
	float cVisibility = 0.0;
	if(depth >= 1.0)
		cVisibility = 1.0;
	
	vec3 bgColor = clearColor * cVisibility;
	
	color.rgb = texture(fColor, uv).rgb * (ambientLight + sDirectionalLight) + bgColor;
	color.a = 1.0-cVisibility;
}