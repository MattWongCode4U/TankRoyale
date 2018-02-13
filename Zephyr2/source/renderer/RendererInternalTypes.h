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

struct ForwardSetup
{
	GLuint program;
	GLuint programMVPM;
	GLuint programTexture;
	GLuint programCameraPos;
	GLuint programAmbient;
	GLuint programDLight;
	GLuint programDLightFacing;
	GLuint programOffsets;
	GLuint programAnimated;
	GLuint programSmoothness;
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
