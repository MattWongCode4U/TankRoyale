#version 330 core
in vec2 uv;
uniform sampler2D sBuffer; //current smearbuffer
uniform sampler2D lBuffer; //last framebuffer
uniform float blurAmount;
uniform float factor;
void main()
{
	vec3 lColor = texture(lBuffer, uv).rgb;	
	
	//repeat "blur" blend
	vec3 sColor = texture(sBuffer, uv).rgb;
	vec3 iColor = mix(lColor, sColor, blurAmount);
	
	//blend into smearbuffer
	gl_FragColor.rgb = mix(iColor, sColor, factor);
	//gl_FragColor.rgb = mix(lColor, sColor, blurAmount);
	//gl_FragColor.rgb = iColor;
	gl_FragColor.a = 1.0;
}