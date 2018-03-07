#pragma once
#include <cstdint>
#include <string>
#include <map>

class GlobalPrefs
{
private:
	static void assignBool(bool &dest, const std::string &key, const std::map<std::string, std::string> &input);
	static void assignInt(int32_t &dest, const std::string &key, const std::map<std::string, std::string> &input);
	static void assignFloat(float &dest, const std::string &key, const std::map<std::string, std::string> &input);
	static void assignString(std::string &dest, const std::string &key, const std::map<std::string, std::string> &input);
public:
	//load method
	static void load();

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
	static bool rEnableShadows;
	static bool rEnablePost;

	//UI vars
	static float uiWidth;
	static float uiHeight;

	//Audio vars
	static float aMasterVolume;
};

