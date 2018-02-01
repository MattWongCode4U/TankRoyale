#pragma once

#include <glew.h>
#include <glm.hpp>
#include <string>
#include <vector>
#include <map>

//TODO move some of these into RendererInternalTypes.h

/// <summary>
/// Types of lights that can be rendered
/// </summary>
enum class RenderableType : int_fast8_t
{
	OBJECT3D, FORWARD3D, BILLBOARD, OVERLAY
};

/// <summary>
/// Types of lights that can be rendered
/// </summary>
enum class RenderableLightType : int_fast8_t
{
	AMBIENT, DIRECTIONAL, POINT, SPOT
};


