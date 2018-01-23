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