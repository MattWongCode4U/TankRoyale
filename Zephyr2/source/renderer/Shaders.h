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
	@see Shaders.cpp
	
*//*====================================================================================*/

/*========================================================================================
	Dependencies
========================================================================================*/
#pragma once

#ifdef __APPLE__
#include <GL/glew.h>
#include <SDL2/SDL.h>
#elif defined _WIN32 || defined _WIN64
#include <glew.h>
#include <SDL.h>
#endif
#include <iostream>

/*========================================================================================
	Shaders	
========================================================================================*/
/**
	Utility class for loading shaders.
	
	@see Shaders
	@see Shaders.cpp
*/
class Shaders
{
    /*------------------------------------------------------------------------------------
		Class Fields
    ------------------------------------------------------------------------------------*/
	private:
		static std::string VSH_01_PATH;
		static std::string FSH_01_PATH;
		static std::string VSH_02_PATH;
		static std::string FSH_02_PATH;
		static std::string VSH_SHADOW_PATH;
		static std::string FSH_SHADOW_PATH;
		static std::string VSH_POINT_PATH;
		static std::string FSH_POINT_PATH;
		static std::string VSH_SPOT_PATH;
		static std::string FSH_SPOT_PATH;
		static std::string VSH_POST_PATH;
		static std::string FSH_POST_PATH;
		static std::string VSH_COPY_PATH;
		static std::string FSH_COPY_PATH;
		
    /*------------------------------------------------------------------------------------
		Class Methods
    ------------------------------------------------------------------------------------*/
	public:
		static GLuint LoadShaders();
		static GLuint LoadShadersFBDraw();
		static GLuint LoadShadersShadows();
		static GLuint LoadShadersPointPass();
		static GLuint LoadShadersSpotPass();
		static GLuint LoadShadersPostProcessing();
		static GLuint LoadShadersSBCopy();
	private:
		static GLuint LoadShadersGeneric(std::string vshPath, std::string fshPath);
};
