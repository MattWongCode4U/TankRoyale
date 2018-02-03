#version 330 core
uniform sampler2D iTexture;
in vec2 oTexC;

void main()
{
	gl_FragColor = texture(iTexture, vec2(oTexC.x, 1.0-oTexC.y));
}