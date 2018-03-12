#version 330 core
in vec2 uv;
uniform sampler2D fBuffer; //normal framebuffer

void main()
{
	//output is always opaque
	gl_FragColor.a = 1.0;

	//just copy
	gl_FragColor.rgb = texture(fBuffer, uv).rgb;
	
}

