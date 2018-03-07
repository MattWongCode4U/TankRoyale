#include "GlobalPrefs.h"
#include <vector>
#include <string>
#include <SDL.h>
#include "Util.h"

//*****DEFAULTS

//Window vars
bool GlobalPrefs::windowFullscreen = false;
int32_t GlobalPrefs::windowWidth = 1280;
int32_t GlobalPrefs::windowHeight = 720;

//Renderer vars
int32_t GlobalPrefs::renderWidth = -1; //-1 means "use window size"
int32_t GlobalPrefs::renderHeight = -1;
int32_t GlobalPrefs::rShadowMapSize = 4096;
float GlobalPrefs::rShadowMapSide = 100.0f;
float GlobalPrefs::rShadowMapNear = 10.0f;
float GlobalPrefs::rShadowMapFar = 100.0f;
float GlobalPrefs::rBlurFactor = 0.6f;
float GlobalPrefs::rBlurAmount = 0.7f;
float GlobalPrefs::rDofAmount = 0.75f;
float GlobalPrefs::rDofFactor = 0.75f;
float GlobalPrefs::rFogAmount = 0.0f;
float GlobalPrefs::rFogFactor = 0.1f;
bool GlobalPrefs::rEnableShadows = true;
bool GlobalPrefs::rEnablePost = false;

//UI vars
float GlobalPrefs::uiWidth = 1280;
float GlobalPrefs::uiHeight = 720;

//Audio vars
float GlobalPrefs::aMasterVolume = 0.75f;

//*****LOAD METHOD
void GlobalPrefs::load()
{
	//this is going to be "fun" in a language without reflection
	//the other option would be to use a map, but then we're back to being stringly typed...

	std::map<std::string, std::string> dataMap;
	try
	{
		//load using util
		std::string rawConfigData = openFileFromAsset("config.txt", ASSET_TYPE::DATA, true);

		//abort on empty/near empty config file
		if (rawConfigData.length() < 3)
		{
			throw std::exception();
		}

		//splitting code adapted from GameSystem
		std::vector<std::string> splitObjData = split(rawConfigData, ',');
		
		for (int i = 0; i < splitObjData.size(); i++) {
			std::vector<std::string> keyValue = split(splitObjData[i], ':');
			dataMap[keyValue[0]] = keyValue[1];
		}
	}
	catch(const std::exception& e)
	{
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to load config!");
		SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, e.what());
		return;
	}

	//assign window settings
	assignBool(windowFullscreen, "windowFullscreen", dataMap);
	assignInt(windowWidth, "windowWidth", dataMap);
	assignInt(windowHeight, "windowHeight", dataMap);

	//assign renderer settings
	assignInt(renderWidth, "renderWidth", dataMap);
	assignInt(renderHeight, "renderHeight", dataMap);
	assignInt(rShadowMapSize, "rShadowMapSize", dataMap);
	assignFloat(rShadowMapSide, "rShadowMapSide", dataMap);
	assignFloat(rShadowMapNear, "rShadowMapNear", dataMap);
	assignFloat(rShadowMapFar, "rShadowMapFar", dataMap);
	assignFloat(rBlurAmount, "rBlurAmount", dataMap);
	assignFloat(rBlurFactor, "rBlurFactor", dataMap);
	assignFloat(rDofAmount, "rDofAmount", dataMap);
	assignFloat(rDofFactor, "rDofFactor", dataMap);
	assignFloat(rFogAmount, "rFogAmount", dataMap);
	assignFloat(rFogFactor, "rFogFactor", dataMap);
	assignBool(rEnableShadows, "rEnableShadows", dataMap);
	assignBool(rEnablePost, "rEnablePost", dataMap);	

	//assign ui settings
	assignFloat(uiWidth, "uiWidth", dataMap);
	assignFloat(uiHeight, "uiHeight", dataMap);

	//assign audio settings
	assignFloat(aMasterVolume, "aMasterVolume", dataMap);
}

void GlobalPrefs::assignBool(bool & dest, const std::string & key, const std::map<std::string, std::string>& input)
{
	if (input.count(key))
	{
		try
		{
			int temp = std::stoi(input.at(key));
			dest = temp;
		}
		catch (...)
		{
			SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to parse key %s",key);
		}

	}
}

void GlobalPrefs::assignInt(int32_t & dest, const std::string & key, const std::map<std::string, std::string>& input)
{
	if (input.count(key))
	{
		try
		{
			int temp = std::stoi(input.at(key));
			dest = temp;
		}
		catch (...)
		{
			SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to parse key %s", key);
		}

	}
}

void GlobalPrefs::assignFloat(float & dest, const std::string & key, const std::map<std::string, std::string>& input)
{
	if (input.count(key))
	{
		try
		{
			float temp = std::stof(input.at(key));
			dest = temp;
		}
		catch (...)
		{
			SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to parse key %s", key);
		}

	}
}

void GlobalPrefs::assignString(std::string & dest, const std::string & key, const std::map<std::string, std::string>& input)
{
	if (input.count(key))
	{
		try
		{
			dest = input.at(key);
		}
		catch (...)
		{
			SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to parse key %s", key);
		}

	}
}