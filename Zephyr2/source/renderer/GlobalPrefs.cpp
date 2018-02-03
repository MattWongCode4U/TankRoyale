#include "GlobalPrefs.h"

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

//UI vars
float GlobalPrefs::uiWidth = 1280;
float GlobalPrefs::uiHeight = 720;