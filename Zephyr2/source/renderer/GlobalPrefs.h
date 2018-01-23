#pragma once
#include <cstdint>

class GlobalPrefs
{
public:

	//Window vars
	static int32_t windowWidth;
	static int32_t windowHeight;
	static bool windowFullscreen;

	//Renderer vars
	static int32_t renderWidth;
	static int32_t renderHeight;
	static int32_t rShadowMapSize;
	static float rShadowMapSide;
	static float rShadowMapNear;
	static float rShadowMapFar;
	static float rBlurFactor;
	static float rBlurAmount;
	static float rDofAmount;
	static float rDofFactor;
	static float rFogAmount;
	static float rFogFactor;
};

