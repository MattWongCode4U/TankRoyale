#pragma once
#ifdef __APPLE__
#include <GL/glew.h>
#include <glm/glm.hpp>
#elif defined _WIN32 || defined _WIN64
#include <glew.h>
#include <glm.hpp>
#endif
#include <vector>

enum class RendererState : int_fast8_t
{
	idle, loading, rendering, unloading
};

struct ModelData
{
	//std::string name; //don't need this, will be stored in hashtable
	GLuint vboID;
	GLuint vaoID;
	GLuint numVerts;
	GLfloat preScale;
};

struct TextureData
{
	//std::string name; //don't need this, will be stored in hashtable
	GLuint texID; 
};

struct ModelLoadingData
{
	//TODO data to be stored while waiting for it to load
	size_t hash;
	std::string name;
};

struct TextureLoadingData
{
	//TODO data to be stored while waiting for it to load
	size_t hash;
	std::string name;
};

struct FramebufferSetup
{
	GLuint fbo;
	GLuint texture0; //color/smoothness
	GLuint texture1; //position
	GLuint texture2; //normal
	GLuint depth;
};

struct FullscreenQuadSetup
{
	GLuint vao;
	GLuint vbo;
	GLuint vertices;
};

struct GeometryPassSetup
{
	GLuint program;
	GLuint programMM;
	GLuint programMVPM;
	GLuint programTexture;
	GLuint programNormal;
	GLuint programHasNorm;
	GLuint programSmoothness;
};

struct ShadowPassSetup
{
	GLuint program;
	GLuint programMM;
	GLuint programMVPM;

	GLuint fbo;
	GLuint texture;
};

struct LightingPassSetup
{
	GLuint program;
	GLuint programTexture0;
	GLuint programTexture1;
	GLuint programTexture2;
	GLuint programTexture3; //depth
	GLuint programTextureS; //shadow
	GLuint programColor;
	GLuint programAmbient;
	GLuint programDirColor;
	GLuint programDirFacing;
	GLuint programCameraPos;
	GLuint programBiasM;

};

struct PointLightPassSetup
{
	GLuint program;
	GLuint programTexture0;
	GLuint programTexture1;
	GLuint programTexture2;
	GLuint programTexture3;
	GLuint programCameraPos;
	GLuint programLightPos;
	GLuint programLightIntensity;
	GLuint programLightColor;
	GLuint programLightRange;
};

struct SpotLightPassSetup
{
	GLuint program;
	GLuint programTexture0;
	GLuint programTexture1;
	GLuint programTexture2;
	GLuint programTexture3;
	GLuint programCameraPos;
	GLuint programLightPos;
	GLuint programLightDir;
	GLuint programLightIntensity;
	GLuint programLightColor;
	GLuint programLightRange;
	GLuint programLightAngle;
};

struct ForwardPassSetup
{
	GLuint program;
	GLuint programMM;
	GLuint programMVM;
	GLuint programPM;
	GLuint programBillboard;
	GLuint programTexture;
	GLuint programCameraPos;
	GLuint programAmbient;
	GLuint programDLight;
	GLuint programDLightFacing;
	GLuint programOffsets;
	GLuint programAnimated;
	GLuint programSmoothness;
};

struct PostProcessingSetup
{
	GLuint program;
	GLuint programTexture;
	GLuint programSmearTexture;
	GLuint programDepthTexture;
	GLuint programBlurAmount;
	GLuint programDofAmount;
	GLuint programDofFactor;
	GLuint programFogAmount;
	GLuint programFogFactor;
	GLuint programFogColor;

	GLuint copyProgram;
	GLuint copyProgramFactor;
	GLuint copyProgramBlurAmount;
	GLuint copyProgramLastTexture;
	GLuint copyProgramSmearTexture;

	GLuint fbo;
	GLuint texture;

	GLuint smearFbo;
	GLuint smearTexture;
};

struct PostBypassSetup
{
	GLuint program;
	GLuint programTexture;
};

struct OverlaySetup
{
	GLuint vao;
	GLuint vbo;

	GLuint program;
	GLuint programMVPM;
	GLuint programTexture;
	GLuint programOffsets;
	GLuint programAnimated;
	glm::mat4 MVPM;
};


/// <summary>
/// Single object data sent to RenderEngine on draw
/// </summary>
struct RenderableObject
{
	RenderableType type;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::string modelName;

	std::string albedoName;
	std::string normalName;
	GLfloat smoothness;

	int frameCount;
	int frameDelay;	
	bool animateOnce;

	int_least64_t startFrame;
};

/// <summary>
/// Single light data sent to RenderEngine on draw
/// </summary>
struct RenderableLight
{
	RenderableLightType type;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 color;
	GLfloat range;
	GLfloat intensity;
	GLfloat angle;
};

/// <summary>
/// Camera data sent to RenderEngine on draw
/// </summary>
struct RenderableCamera
{
	glm::vec3 position;
	glm::vec3 rotation;

	glm::vec3 clearColor;
	GLfloat viewAngle;
	GLfloat nearPlane;
	GLfloat farPlane;
};

/// <summary>
/// Model information sent to the RenderEngine to load
/// </summary>
struct RenderableModel
{
	std::string name;
};

/// <summary>
/// Texture information sent to the RenderEngine to load
/// </summary>
struct RenderableTexture
{
	std::string name;
};

/// <summary>
/// Data sent to RenderEngine on scene load
/// </summary>
struct RenderableSetupData
{
	std::vector<std::string> models;
	std::vector<std::string> textures;
};

/// <summary>
/// Data sent to RenderEngine to draw
/// </summary>
struct RenderableScene
{
	std::vector<RenderableObject> objects;
	std::vector<RenderableObject> forwardObjects;
	std::vector<RenderableObject> billboardObjects;
	std::vector<RenderableLight> lights;
	//TODO separate out maincamera and such
	RenderableCamera camera;
};

/// <summary>
/// Data sent to RenderEngine to draw an overlay (WIP, currently empty)
/// </summary>
struct RenderableOverlay
{
	std::vector<RenderableObject> elements;
};
