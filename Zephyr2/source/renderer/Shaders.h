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

#include <glew.h>
#include <SDL.h>
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
		static std::string VSH_OVERLAY_PATH;
		static std::string FSH_OVERLAY_PATH;
		static std::string VSH_FORWARD_PATH;
		static std::string FSH_FORWARD_PATH;
		static std::string VSH_BILLBOARD_PATH;
		static std::string FSH_BILLBOARD_PATH;
		
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
		static GLuint LoadShadersOverlay();
		static GLuint LoadShadersForward();
		static GLuint LoadShadersBillboard();
	private:
		static GLuint LoadShadersGeneric(std::string vshPath, std::string fshPath);
};
