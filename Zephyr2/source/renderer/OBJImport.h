#pragma once

#include <glew.h>
#include <SDL.h>
#include <SDL_filesystem.h>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>

class OBJImport {
public:

	/// <summary>
	/// Loads info from an .obj file to vector GLfloat array to use for render engine
	///
	/// </summary>
	static std::vector<GLfloat> importObjInfo(std::string string);
	static std::vector<GLfloat> importObjInfoVertices(std::string string);
};