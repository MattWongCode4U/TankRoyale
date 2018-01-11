#version 330 core
in vec2 uv;                     // UV coordinates of the texture.
out vec3 color;                 // Output color.
uniform sampler2D fColor;       // Input color.
uniform sampler2D fPosition;    // Position factor. Combine with UV to get vertex position.
uniform sampler2D fNormal;      // Normal factor. Combine with UV to get vertex normal.
uniform sampler2D fDepth;       // Depth factor.
uniform vec3 cameraPos;         // Camera position.
uniform vec3 lightColor;        // Base light color from point light. Multiply with light intensity to get the effective light.
uniform vec3 lightPos;          // Point light position.
uniform float lightRange;       // Maximum range for this light. Vertices further than this from the light will not be lit by it.
uniform float lightIntensity;   // Light intensity. Multiply with base light color to get the effective light.

// Fragment shader for point lights.
void main()
{
	float smoothness = texture(fColor, uv).a;       // The smoothness is stored in the a component of the texture.
	vec3 image = texture( fColor, uv).rgb;          // The surface color is stored in the rgb components of the texture.
    vec3 position = texture( fPosition, uv).rgb;    // Get fragment position.
    vec3 normal = texture( fNormal, uv).rgb;        // Get fragment normal.
	
	vec3 lightColorFac = lightColor * lightIntensity;   // Scale light by its intensity.
	vec3 lightVec = lightPos - position;                // Get the vector from the fragment to the light source.
	vec3 lightDir = normalize(lightVec);                // Get the direction of the light vector.
	float lightDist = length(lightVec);                 // Get the length of the light vector.
	normal = normalize(normal);                         // Get the normal direction.
	
	vec3 eyeDir = normalize(cameraPos - position);      // Get vector from the fragment to the camera.
	vec3 vHalfVec = normalize(lightDir + eyeDir);       // Get the direction of the halfway vector between the light direction and the eye direction.

	float attn = pow(clamp(1.0 - lightDist/lightRange, 0.0, 1.0), 2);   // Squared ratio of distance from light to max light range.
	
	float diffuseC = max(0.0, dot(normal, lightDir));       // Determine how much of the light reflection will be diffuse based on surface angle.
	float diffuse = diffuseC * attn * (1.0 - smoothness);   // Combine surface angle, attenuation, and diffuse ratio (1 minus specular) to get the degree of diffuse reflection.
	
	float spec = 0.0;   // Initialize specular component to zero.

    // If the diffuse component is greater than 0, set the specular component.
	if(diffuseC > 0)
	{
		spec = pow(                             // Specular coefficient
                    max(                        // Cosine angle
                        0.0, 
                        dot(
                            eyeDir, 
                            reflect(            // Reflection vector
                                -lightDir,      // Incidence vector
                                normal))), 
                    50) 
                    * attn 
                    * smoothness;
	}

	color = (diffuse * lightColorFac) + (spec * lightColorFac);     // The ouput is the weighted sum of diffuse and specular reflection.
}