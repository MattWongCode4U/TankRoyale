#pragma once
#ifdef __APPLE__
#include <GL/glew.h>
#include <glm/glm.hpp>
#elif defined _WIN32 || defined _WIN64
#include <glew.h>
#include <glm.hpp>
#endif
#include <string>
#include <vector>
#include <map>

/// <summary>
/// Types of lights that can be rendered
/// </summary>
enum class RenderableLightType : int_fast8_t
{
	AMBIENT, DIRECTIONAL, POINT, SPOT
};

/// <summary>
/// Single object data sent to RenderEngine on draw
/// </summary>
struct RenderableObject
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	std::string modelName;

	std::string albedoName;
	std::string normalName;
	GLfloat smoothness;
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
	std::vector<RenderableLight> lights;
	RenderableCamera camera;
};

/// <summary>
/// Data sent to RenderEngine to draw an overlay (WIP, currently empty)
/// </summary>
struct RenderableOverlay
{

};


