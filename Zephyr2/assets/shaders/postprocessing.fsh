#version 330 core
in vec2 uv;
uniform sampler2D fBuffer; //normal framebuffer
uniform sampler2D dBuffer; //depth buffer2
uniform sampler2D sBuffer; //smear buffer2
uniform float blurAmount;
uniform float dofAmount;
uniform float dofFactor;
uniform float fogAmount;
uniform float fogFactor;
uniform vec3 fogColor;

vec3 blurSampleGaussian(float dofblur, sampler2D buffer2)
{
	dofblur = clamp(dofblur * 0.25, 0.0, 0.003);

	vec4 col = vec4(0.0);
	
	//row 0	
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.000072;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.000323;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.000944;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.001794;
	col += texture(buffer2, vec2(uv.x, uv.y + 4.0 * dofblur)) * 0.002222;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.001794;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.000944;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.000323;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y + 4.0 * dofblur)) * 0.000072;
	
	//row 1
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.000323;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.00145;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x, uv.y + 3.0 * dofblur)) * 0.00997;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.00145;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y + 3.0 * dofblur)) * 0.000323;
	
	//row 2
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.000944;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.012358;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x, uv.y + 2.0 * dofblur)) * 0.029106;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.012358;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y + 2.0 * dofblur)) * 0.000944;
	
	//row 3
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.001794;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.044672;
	col += texture(buffer2, vec2(uv.x, uv.y + 1.0 * dofblur)) * 0.055338;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.044672;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y + 1.0 * dofblur)) * 0.001794;
	
	//row 4
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y)) * 0.002222;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y)) * 0.00997;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y)) * 0.029106;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y)) * 0.055338;
	col += texture(buffer2, vec2(uv.x, uv.y)) * 0.068552;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y)) * 0.055338;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y)) * 0.029106;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y)) * 0.00997;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y)) * 0.002222;
	
	//row 5
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.001794;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.044672;
	col += texture(buffer2, vec2(uv.x, uv.y - 1.0 * dofblur)) * 0.055338;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.044672;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y - 1.0 * dofblur)) * 0.001794;
	
	//row 6
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.000944;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.012358;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x, uv.y - 2.0 * dofblur)) * 0.029106;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.023496;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.012358;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y - 2.0 * dofblur)) * 0.000944;
	
	//row 7
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.000323;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.00145;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x, uv.y + 3.0 * dofblur)) * 0.00997;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.008048;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.004233;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.00145;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y - 3.0 * dofblur)) * 0.000323;
	
	//row 8
	col += texture(buffer2, vec2(uv.x - 4.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.000072;
	col += texture(buffer2, vec2(uv.x - 3.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.000323;
	col += texture(buffer2, vec2(uv.x - 2.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.000944;
	col += texture(buffer2, vec2(uv.x - 1.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.001794;
	col += texture(buffer2, vec2(uv.x, uv.y - 4.0 * dofblur)) * 0.002222;
	col += texture(buffer2, vec2(uv.x + 1.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.001794;
	col += texture(buffer2, vec2(uv.x + 2.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.000944;
	col += texture(buffer2, vec2(uv.x + 3.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.000323;
	col += texture(buffer2, vec2(uv.x + 4.0 * dofblur, uv.y - 4.0 * dofblur)) * 0.000072;
	
	return col.rgb;
}

void main()
{
	vec3 fColor = texture(fBuffer, uv).rgb;
	vec3 sColor = texture(sBuffer, uv).rgb;
	float depth = texture(dBuffer, uv).r;
	
	//output is always opaque
	gl_FragColor.a = 1.0;
	
	//depth of field
	float cDepth = texture(dBuffer, vec2(0.5, 0.25)).r;
	float depthDiff = clamp(abs(depth - cDepth), 0, 1.0);
	float biasedDepth = depthDiff * 10.0;
	vec3 blurColor = blurSampleGaussian(biasedDepth * 0.1 * dofFactor, fBuffer);
	
	//fog
	float fogDist = depth * 10.0;
	float fogValue = clamp( exp(-fogFactor*fogDist), 0.0, 1.0);

	//blend smearbuffer, then DoF, then fog
	gl_FragColor.rgb = mix(mix(mix(fColor, sColor, blurAmount), blurColor, dofAmount), fogColor, fogValue * fogAmount);
	
}

