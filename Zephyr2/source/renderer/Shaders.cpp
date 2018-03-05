/*===================================================================================*//**
	Shaders
	
	Utility class for loading shaders.
    
    Copyright 2017 Erick Fernandez de Arteaga. All rights reserved.
        https://www.linkedin.com/in/erick-fda
        https://bitbucket.org/erick-fda

	@author Chris Leclair, Erick Fernandez de Arteaga
	@version 0.0.0
	@file
	
	@see Shaders
	@see Shaders.h
	
*//*====================================================================================*/

/*========================================================================================
	Dependencies
========================================================================================*/
#include "Shaders.h"
#include "../Util.h"

/*========================================================================================
	Class Fields
========================================================================================*/
std::string Shaders::VSH_01_PATH = "assets/shaders/shader01.vsh";
std::string Shaders::FSH_01_PATH = "assets/shaders/shader01.fsh";
std::string Shaders::VSH_02_PATH = "assets/shaders/shader02.vsh";
std::string Shaders::FSH_02_PATH = "assets/shaders/shader02.fsh";
std::string Shaders::VSH_SHADOW_PATH = "assets/shaders/shadow.vsh";
std::string Shaders::FSH_SHADOW_PATH = "assets/shaders/shadow.fsh";
std::string Shaders::VSH_POINT_PATH = "assets/shaders/light_point.vsh";
std::string Shaders::FSH_POINT_PATH = "assets/shaders/light_point.fsh";
std::string Shaders::VSH_SPOT_PATH = "assets/shaders/light_spot.vsh";
std::string Shaders::FSH_SPOT_PATH = "assets/shaders/light_spot.fsh";
std::string Shaders::VSH_POST_PATH = "assets/shaders/postprocessing.vsh";
std::string Shaders::FSH_POST_PATH = "assets/shaders/postprocessing.fsh";
std::string Shaders::VSH_COPY_PATH = "assets/shaders/sbcopy.vsh";
std::string Shaders::FSH_COPY_PATH = "assets/shaders/sbcopy.fsh";
std::string Shaders::VSH_BYPASS_PATH = "assets/shaders/postbypass.vsh";
std::string Shaders::FSH_BYPASS_PATH = "assets/shaders/postbypass.fsh";
std::string Shaders::VSH_OVERLAY_PATH = "assets/shaders/overlay.vsh";
std::string Shaders::FSH_OVERLAY_PATH = "assets/shaders/overlay.fsh";
std::string Shaders::VSH_FORWARD_PATH = "assets/shaders/forward.vsh";
std::string Shaders::FSH_FORWARD_PATH = "assets/shaders/forward.fsh";
std::string Shaders::VSH_BILLBOARD_PATH = "assets/shaders/billboard.vsh";
std::string Shaders::FSH_BILLBOARD_PATH = "assets/shaders/billboard.fsh";

/*----------------------------------------------------------------------------------------
	Class Methods
----------------------------------------------------------------------------------------*/
///
///
///
GLuint Shaders::LoadShaders()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_01_PATH, Shaders::FSH_01_PATH);
}

///
///
///
GLuint Shaders::LoadShadersFBDraw()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_02_PATH, Shaders::FSH_02_PATH);
}

///
///
///
GLuint Shaders::LoadShadersShadows()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_SHADOW_PATH, Shaders::FSH_SHADOW_PATH);
}

///
///
///
GLuint Shaders::LoadShadersPointPass()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_POINT_PATH, Shaders::FSH_POINT_PATH);
}

///
///
///
GLuint Shaders::LoadShadersSpotPass()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_SPOT_PATH, Shaders::FSH_SPOT_PATH);
}

///
///
///
GLuint Shaders::LoadShadersPostProcessing()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_POST_PATH, Shaders::FSH_POST_PATH);
}

///
///
///
GLuint Shaders::LoadShadersPostBypass()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_BYPASS_PATH, Shaders::FSH_BYPASS_PATH);
}

///
///
///
GLuint Shaders::LoadShadersSBCopy()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_COPY_PATH, Shaders::FSH_COPY_PATH);
}

///
///
///
GLuint Shaders::LoadShadersOverlay()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_OVERLAY_PATH, Shaders::FSH_OVERLAY_PATH);
}

///
///
///
GLuint Shaders::LoadShadersForward()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_FORWARD_PATH, Shaders::FSH_FORWARD_PATH);
}

///
///
///
GLuint Shaders::LoadShadersBillboard()
{
	return Shaders::LoadShadersGeneric(Shaders::VSH_BILLBOARD_PATH, Shaders::FSH_BILLBOARD_PATH);
}

///
///
///
GLuint Shaders::LoadShadersGeneric(std::string vshPath, std::string fshPath)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vsh = openFile(vshPath);
	const char* vshContent = vsh.c_str();
	glShaderSource(vertexShader, 1, &(vshContent), NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		SDL_Log("%s", infoLog);
	}

	// fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fsh = openFile(fshPath);
	const char* fshContent = fsh.c_str();
	glShaderSource(fragmentShader, 1, &(fshContent), NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		SDL_Log("%s", infoLog);
	}

	// link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		SDL_Log("%s", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}
