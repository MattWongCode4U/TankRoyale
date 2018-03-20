#include "RenderPipeline.h"

#include <glew.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtx\euler_angles.hpp>
#include <SDL.h>
#include <SDL_image.h>

#include "..\Util.h"
#include "..\GlobalPrefs.h"
#include "Shaders.h"
#include "Quad.h"
#include "OBJImport.h"
//#include "Cube.h";

//TODO move these
#ifndef FILE_CONSTANTS
#define FILE_CONSTANTS
const std::string MODEL_EXTENSION_CONST = ".obj";
const std::string TEXTURE_EXTENSION_CONST = ".png";
#endif

const int_least64_t IDLE_DELAY_CONST = 10;

RenderPipeline::RenderPipeline(SDL_Window *window_p)
{
	_models_p = new std::map<std::string, ModelData>();
	_textures_p = new std::map<std::string, TextureData>();
	_window_p = window_p;
	_frameCount = 0;
}

RenderPipeline::~RenderPipeline()
{

	delete(_textures_p);
	delete(_models_p);
}

/// <summary>
/// Helper method for initial OpenGL setup
/// Creates OpenGL context and initializes GLEW
/// </summary>
void RenderPipeline::setupGLOnThread()
{
	//this should work in theory as long as we don't need a GL context on the main thread
	//which would slow things down
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	//SDL_GL_CreateContext(_window_p);
	_context_p = SDL_GL_CreateContext(_window_p); //we will need to modify this to release/acquire context in concert with the UI thread
	SDL_GL_SetSwapInterval(1);
	//SDL_GL_MakeCurrent(g_window_p, _context_p);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		SDL_Log("Renderer: %s\n", glewGetErrorString(err));
	}
	//SDL_Log((char*)glGetString(GL_VERSION));
	//SDL_Log((char*)glGetString(GL_RENDERER));
}

/// <summary>
/// Helper method for initial scene setup
/// Confusingly named; actually handles window, shaders, G-buffer, and temporary cube
/// </summary>
void RenderPipeline::setupSceneOnThread()
{
	setupWindow();
	setupProgram();
	setupFramebuffers();
	setupFramebufferDraw();
	setupShadowMapping();
	setupForward();
	setupPostProcessing();
	setupOverlay();
	setupFallbacks();
}

/// <summary>
/// Helper method for final scene cleanup
/// Confusingly named; actually handles window, shaders, G-buffer, and temporary cube
/// </summary>
void RenderPipeline::cleanupSceneOnThread()
{
	cleanupProgram();
	cleanupFramebuffers();
	cleanupFramebufferDraw();
	cleanupShadowMapping();
	cleanupForward();
	cleanupPostProcessing();
	cleanupOverlay();
	cleanupFallbacks();
}

/// <summary>
/// Helper method for final scene cleanup
/// Confusingly named; actually handles window, shaders, G-buffer, and temporary cube
/// </summary>
void RenderPipeline::cleanupGLOnThread()
{
	//TODO destroy the openGL context
}

/// <summary>
/// Helper method for initial setup
/// Gets render width and height
/// </summary>
void RenderPipeline::setupWindow()
{
	//eventually this may do more

	int width, height;
	if (GlobalPrefs::renderHeight > 0 && GlobalPrefs::renderWidth > 0)
	{
		width = GlobalPrefs::renderWidth;
		height = GlobalPrefs::renderHeight;
	}
	else
	{
		SDL_GL_GetDrawableSize(_window_p, &width, &height);
	}

	_renderWidth = width;
	_renderHeight = height;
}

/// <summary>
/// Helper method initial setup
/// Loads shaders and gets uniform locations
/// </summary>
void RenderPipeline::setupProgram()
{
	_geometryPassData.program = Shaders::LoadShaders();
	_geometryPassData.programMM = glGetUniformLocation(_geometryPassData.program, "iModelMatrix");
	_geometryPassData.programMVPM = glGetUniformLocation(_geometryPassData.program, "iModelViewProjectionMatrix");
	_geometryPassData.programTexture = glGetUniformLocation(_geometryPassData.program, "iTexImage");
	_geometryPassData.programNormal = glGetUniformLocation(_geometryPassData.program, "iNormImage");
	_geometryPassData.programHasNorm = glGetUniformLocation(_geometryPassData.program, "iHasNorm");
	_geometryPassData.programSmoothness = glGetUniformLocation(_geometryPassData.program, "iSmoothness");
}

/// <summary>
/// Helper method for final cleanup
/// Deletes shaders
/// </summary>
void RenderPipeline::cleanupProgram()
{
	//delete shaders/program
	if (_geometryPassData.program > 0)
		glDeleteProgram(_geometryPassData.program);
}

/// <summary>
/// Helper method initial setup
/// Set up shaders and quad for overlay
/// </summary>
void RenderPipeline::setupOverlay()
{
	//set up shader
	_overlayDrawData.program = Shaders::LoadShadersOverlay();
	_overlayDrawData.programMVPM = glGetUniformLocation(_overlayDrawData.program, "iMVPM");
	_overlayDrawData.programTexture = glGetUniformLocation(_overlayDrawData.program, "iTexture");
	_overlayDrawData.programAnimated = glGetUniformLocation(_overlayDrawData.program, "iAnimated");
	_overlayDrawData.programOffsets = glGetUniformLocation(_overlayDrawData.program, "iOffsets");

	//set up base MVPm
	float halfUIWidth = GlobalPrefs::uiWidth / 2.0f;
	float halfUIHeight = GlobalPrefs::uiHeight / 2.0f;
	glm::mat4 projection = glm::ortho<float>(-halfUIWidth, halfUIWidth, -halfUIHeight, halfUIHeight, 0.1f, 1000.0f);
	glm::mat4 look = glm::lookAt(glm::vec3(0, 0, 100), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	_overlayDrawData.MVPM = projection * look;

	//set up quad
	glGenVertexArrays(1, &_overlayDrawData.vao);
	glBindVertexArray(_overlayDrawData.vao);

	glGenBuffers(1, &_overlayDrawData.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _overlayDrawData.vbo);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data_tex), g_quad_vertex_buffer_data_tex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0);
}

/// <summary>
/// Helper method initial setup
/// Cleans up shaders and quad for overlay
/// </summary>
void RenderPipeline::cleanupOverlay()
{
	// delete VBOs/VAOs
	glDeleteBuffers(1, &_overlayDrawData.vbo);
	glDeleteVertexArrays(1, &_overlayDrawData.vao);

	//delete programs
	glDeleteProgram(_overlayDrawData.program);
}

/// <summary>
/// Helper method initial setup
/// Set up shaders for forward pass
/// </summary>
void RenderPipeline::setupForward()
{
	_forwardPassData.program = Shaders::LoadShadersForward();
	_forwardPassData.programMVM = glGetUniformLocation(_forwardPassData.program, "iMVM");
	_forwardPassData.programPM = glGetUniformLocation(_forwardPassData.program, "iPM");
	_forwardPassData.programMM = glGetUniformLocation(_forwardPassData.program, "iMM");
	_forwardPassData.programBillboard = glGetUniformLocation(_forwardPassData.program, "iBillboard");
	_forwardPassData.programTexture = glGetUniformLocation(_forwardPassData.program, "iTexture");
	_forwardPassData.programAnimated = glGetUniformLocation(_forwardPassData.program, "iAnimated");
	_forwardPassData.programOffsets = glGetUniformLocation(_forwardPassData.program, "iOffsets");
	_forwardPassData.programAmbient = glGetUniformLocation(_forwardPassData.program, "iAmbient");
	_forwardPassData.programCameraPos = glGetUniformLocation(_forwardPassData.program, "iCameraPos");
	_forwardPassData.programDLight = glGetUniformLocation(_forwardPassData.program, "iDLight");
	_forwardPassData.programDLightFacing = glGetUniformLocation(_forwardPassData.program, "iDLightFacing");
	//_forwardDrawData.programDLightPos = glGetUniformLocation(_forwardDrawData.program, "iDLightPos");
	_forwardPassData.programSmoothness = glGetUniformLocation(_forwardPassData.program, "iSmoothness");
}

/// <summary>
/// Helper method for final cleanup
/// Deletes forward pass program
/// </summary>
void RenderPipeline::cleanupForward()
{
	glDeleteProgram(_forwardPassData.program);
}

/// <summary>
/// Helper method initial setup
/// Loads basic/placeholder texture
/// </summary>
void RenderPipeline::setupFallbacks()
{
	//setup cube (fallback) texture

	std::string texturePath = getAssetPrefixPath(ASSET_TYPE::TEXTURE) + "default" + TEXTURE_EXTENSION_CONST;
	SDL_Surface *image_p = IMG_Load(texturePath.c_str()); //TODO move to an internal helper function

	GLint mode = GL_RGB;
	
	if (image_p->format->BytesPerPixel == 4)
		mode = GL_RGBA;

	GLuint glTexId;

	glGenTextures(1, &glTexId);
	glBindTexture(GL_TEXTURE_2D, glTexId);

	glTexImage2D(GL_TEXTURE_2D, 0, mode, image_p->w, image_p->h, 0, mode, GL_UNSIGNED_BYTE, image_p->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_FreeSurface(image_p);
	image_p = nullptr;

	_fallbackTextureID = glTexId;
}

/// <summary>
/// Helper method for final cleanup
/// Deletes fallback texture object
/// </summary>
void RenderPipeline::cleanupFallbacks()
{
	glDeleteTextures(1, &_fallbackTextureID);
}

/// <summary>
/// Helper method initial setup
/// Sets up FBO and textures for G-buffer
/// </summary>
void RenderPipeline::setupFramebuffers()
{

	//gen FBO
	glGenFramebuffers(1, &_framebufferData.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferData.fbo);

	//gen framebuffer textures
	glGenTextures(1, &_framebufferData.texture0);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _renderWidth, _renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _framebufferData.texture0, 0);

	glGenTextures(1, &_framebufferData.texture1);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _renderWidth, _renderHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _framebufferData.texture1, 0);

	glGenTextures(1, &_framebufferData.texture2);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8_SNORM, _renderWidth, _renderHeight, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _framebufferData.texture2, 0);

	//gen depthbuffer
	glGenTextures(1, &_framebufferData.depth);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _renderWidth, _renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _framebufferData.depth, 0);

	//configure FBO		
	GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, drawBuffers);

	//quick check
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SDL_Log("Renderer: Failed to setup framebuffer!");
		throw;
	}

	//unbind all
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/// <summary>
/// Helper method for final cleanup
/// Deletes G-buffer textures and FBO
/// </summary>
void RenderPipeline::cleanupFramebuffers()
{
	//delete FBOs
	glDeleteTextures(1, &_framebufferData.texture0);
	glDeleteTextures(1, &_framebufferData.texture1);
	glDeleteTextures(1, &_framebufferData.texture2);
	glDeleteTextures(1, &_framebufferData.depth);
	glDeleteFramebuffers(1, &_framebufferData.fbo);
}

/// <summary>
/// Helper method initial setup
/// Sets up fullscreen quad, shaders and uniform locations for lighting pass
/// </summary>
void RenderPipeline::setupFramebufferDraw()
{
	//setup fullscreen quad VAO
	_fullscreenQuadData.vertices = 6;
	glGenVertexArrays(1, &_fullscreenQuadData.vao);
	glBindVertexArray(_fullscreenQuadData.vao);

	glGenBuffers(1, &_fullscreenQuadData.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _fullscreenQuadData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//setup main pass shader
	_lightingPassData.program = Shaders::LoadShadersFBDraw();
	_lightingPassData.programTexture0 = glGetUniformLocation(_lightingPassData.program, "fColor");
	_lightingPassData.programTexture1 = glGetUniformLocation(_lightingPassData.program, "fPosition");
	_lightingPassData.programTexture2 = glGetUniformLocation(_lightingPassData.program, "fNormal");
	_lightingPassData.programTexture3 = glGetUniformLocation(_lightingPassData.program, "fDepth");
	_lightingPassData.programTextureS = glGetUniformLocation(_lightingPassData.program, "sDepth");
	_lightingPassData.programColor = glGetUniformLocation(_lightingPassData.program, "clearColor");
	_lightingPassData.programAmbient = glGetUniformLocation(_lightingPassData.program, "ambientLight");
	_lightingPassData.programDirColor = glGetUniformLocation(_lightingPassData.program, "dLightColor");
	_lightingPassData.programDirFacing = glGetUniformLocation(_lightingPassData.program, "dLightFacing");;
	_lightingPassData.programCameraPos = glGetUniformLocation(_lightingPassData.program, "cameraPos");;
	_lightingPassData.programBiasM = glGetUniformLocation(_lightingPassData.program, "biasMVP");

	//setup point pass shader
	_pointlightPassData.program = Shaders::LoadShadersPointPass();
	_pointlightPassData.programTexture0 = glGetUniformLocation(_pointlightPassData.program, "fColor");
	_pointlightPassData.programTexture1 = glGetUniformLocation(_pointlightPassData.program, "fPosition");
	_pointlightPassData.programTexture2 = glGetUniformLocation(_pointlightPassData.program, "fNormal");
	_pointlightPassData.programTexture3 = glGetUniformLocation(_pointlightPassData.program, "fDepth");
	_pointlightPassData.programCameraPos = glGetUniformLocation(_pointlightPassData.program, "cameraPos");
	_pointlightPassData.programLightColor = glGetUniformLocation(_pointlightPassData.program, "lightColor");
	_pointlightPassData.programLightPos = glGetUniformLocation(_pointlightPassData.program, "lightPos");
	_pointlightPassData.programLightIntensity = glGetUniformLocation(_pointlightPassData.program, "lightIntensity");
	_pointlightPassData.programLightRange = glGetUniformLocation(_pointlightPassData.program, "lightRange");

	//setup spot pass shader
	_spotlightPassData.program = Shaders::LoadShadersSpotPass();
	_spotlightPassData.programTexture0 = glGetUniformLocation(_spotlightPassData.program, "fColor");
	_spotlightPassData.programTexture1 = glGetUniformLocation(_spotlightPassData.program, "fPosition");
	_spotlightPassData.programTexture2 = glGetUniformLocation(_spotlightPassData.program, "fNormal");
	_spotlightPassData.programTexture3 = glGetUniformLocation(_spotlightPassData.program, "fDepth");
	_spotlightPassData.programCameraPos = glGetUniformLocation(_spotlightPassData.program, "cameraPos");
	_spotlightPassData.programLightColor = glGetUniformLocation(_spotlightPassData.program, "lightColor");
	_spotlightPassData.programLightPos = glGetUniformLocation(_spotlightPassData.program, "lightPos");
	_spotlightPassData.programLightDir = glGetUniformLocation(_spotlightPassData.program, "lightFacing");
	_spotlightPassData.programLightIntensity = glGetUniformLocation(_spotlightPassData.program, "lightIntensity");
	_spotlightPassData.programLightRange = glGetUniformLocation(_spotlightPassData.program, "lightRange");
	_spotlightPassData.programLightAngle = glGetUniformLocation(_spotlightPassData.program, "lightAngle");
}

/// <summary>
/// Helper method for final cleanup
/// Deletes shaders
/// </summary>
void RenderPipeline::cleanupFramebufferDraw()
{
	// delete VBOs/VAOs
	glDeleteBuffers(1, &_fullscreenQuadData.vbo);
	glDeleteVertexArrays(1, &_fullscreenQuadData.vao);

	//delete programs
	glDeleteProgram(_lightingPassData.program);
	glDeleteProgram(_pointlightPassData.program);
	glDeleteProgram(_spotlightPassData.program);
}

/// <summary>
/// Helper method initial setup
/// Sets up geometry, buffers, and shaders for shadow mapping
/// </summary>
void RenderPipeline::setupShadowMapping()
{
	/* Set up directional/shadow shader. */
	_shadowPassData.program = Shaders::LoadShadersShadows();
	_shadowPassData.programMM = glGetUniformLocation(_shadowPassData.program, "iModelMatrix");
	_shadowPassData.programMVPM = glGetUniformLocation(_shadowPassData.program, "iModelViewProjectionMatrix");

	/* Generate FBO for shadow depth buffer. */
	glGenFramebuffers(1, &_shadowPassData.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowPassData.fbo);

	/* Generate the shadow depth buffer. */
	glGenTextures(1, &_shadowPassData.texture);
	glBindTexture(GL_TEXTURE_2D, _shadowPassData.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, GlobalPrefs::rShadowMapSize, GlobalPrefs::rShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); //TODO CONST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _shadowPassData.texture, 0);

	glDrawBuffer(GL_NONE);

	/* Unbind the FBO. */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// <summary>
/// Helper method for final cleanup
/// Deletes shadow mapping buffers, programs, and geometry
/// </summary>
void RenderPipeline::cleanupShadowMapping()
{
	glDeleteTextures(1, &_shadowPassData.texture);
	glDeleteFramebuffers(1, &_shadowPassData.fbo);
	glDeleteProgram(_shadowPassData.program);
}

/// <summary>
/// Helper method initial setup
/// Sets up program and buffers for postprocessing
/// </summary>
void RenderPipeline::setupPostProcessing()
{
	//load bypass/alternate shader
	_postBypassData.program = Shaders::LoadShadersPostBypass();
	_postBypassData.programTexture = glGetUniformLocation(_postBypassData.program, "fBuffer");

	//load shader
	_postProcessingData.program = Shaders::LoadShadersPostProcessing();
	_postProcessingData.programTexture = glGetUniformLocation(_postProcessingData.program, "fBuffer");
	_postProcessingData.programSmearTexture = glGetUniformLocation(_postProcessingData.program, "sBuffer");
	_postProcessingData.programDepthTexture = glGetUniformLocation(_postProcessingData.program, "dBuffer");
	_postProcessingData.programBlurAmount = glGetUniformLocation(_postProcessingData.program, "blurAmount");
	_postProcessingData.programDofAmount = glGetUniformLocation(_postProcessingData.program, "dofAmount");
	_postProcessingData.programDofFactor = glGetUniformLocation(_postProcessingData.program, "dofFactor");
	_postProcessingData.programFogAmount = glGetUniformLocation(_postProcessingData.program, "fogAmount");
	_postProcessingData.programFogFactor = glGetUniformLocation(_postProcessingData.program, "fogFactor");
	_postProcessingData.programFogColor = glGetUniformLocation(_postProcessingData.program, "fogColor");

	//load smearbuffer copy shader
	_postProcessingData.copyProgram = Shaders::LoadShadersSBCopy();
	_postProcessingData.copyProgramFactor = glGetUniformLocation(_postProcessingData.copyProgram, "factor");
	_postProcessingData.copyProgramBlurAmount = glGetUniformLocation(_postProcessingData.copyProgram, "blurAmount");
	_postProcessingData.copyProgramLastTexture = glGetUniformLocation(_postProcessingData.copyProgram, "lBuffer");
	_postProcessingData.copyProgramSmearTexture = glGetUniformLocation(_postProcessingData.copyProgram, "sBuffer");

	//generate base framebuffer FBO and texture
	glGenFramebuffers(1, &_postProcessingData.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _postProcessingData.fbo);
	glGenTextures(1, &_postProcessingData.texture);
	glBindTexture(GL_TEXTURE_2D, _postProcessingData.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _renderWidth, _renderHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _postProcessingData.texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//generate smearbuffer FBO and texture
	glGenFramebuffers(1, &_postProcessingData.smearFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _postProcessingData.smearFbo);
	glGenTextures(1, &_postProcessingData.smearTexture);
	glBindTexture(GL_TEXTURE_2D, _postProcessingData.smearTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _renderWidth, _renderHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _postProcessingData.smearTexture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/// <summary>
/// Helper method for final cleanup
/// Deletes postprocessing buffers and program
/// </summary>
void RenderPipeline::cleanupPostProcessing()
{
	glDeleteTextures(1, &_postProcessingData.texture);
	glDeleteFramebuffers(1, &_postProcessingData.fbo);
	glDeleteTextures(1, &_postProcessingData.smearTexture);
	glDeleteFramebuffers(1, &_postProcessingData.smearFbo);

	glDeleteProgram(_postBypassData.program);
	glDeleteProgram(_postProcessingData.program);
	glDeleteProgram(_postProcessingData.copyProgram);
}

/// <summary>
/// Primary method for resource loading
/// Loads, binds, and registers all available models and textures
/// </summary>
void RenderPipeline::loadAllResources()
{
	//TODO implementation
	std::vector<std::string> textureNames = listFilesInPath(getAssetPrefixPath(ASSET_TYPE::TEXTURE));
	for (auto textureName : textureNames) {
		loadOneTexture(textureName);
	}

	std::vector<std::string> modelNames = listFilesInPath(getAssetPrefixPath(ASSET_TYPE::MODEL));
	for (auto modelName : modelNames) {
		loadOneModel(modelName);
	}

	//TODO use a list instead of loading everything?
}

/// <summary>
/// Helper method for model loading
/// Converts and binds a single model, then pushes to model list
/// </summary>
void RenderPipeline::loadOneModel(std::string modelName)
{

	ModelData md;

	std::string path = getAssetPrefixPath(ASSET_TYPE::MODEL) + modelName + MODEL_EXTENSION_CONST;
	std::string data = openFile(path);

	auto objData = OBJImport::importObjInfo(data);

	GLuint numVertices = (GLuint)objData.size() / 8;
	GLfloat *objPtr = &objData[0];
	GLuint glVaoId, glVboId;

	glGenVertexArrays(1, &glVaoId);
	glBindVertexArray(glVaoId);
	glGenBuffers(1, &glVboId);
	glBindBuffer(GL_ARRAY_BUFFER, glVboId);
	glBufferData(GL_ARRAY_BUFFER, (objData.size() * sizeof(GLfloat)), objPtr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0); //vertex coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT))); //normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT))); //UVs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	md.numVerts = numVertices;
	md.preScale = 1.0f;
	md.vaoID = glVaoId;
	md.vboID = glVboId;

	_models_p->emplace(modelName, md);

}

/// <summary>
/// Helper method for texture loading
/// Converts and binds a single texture, then pushes to texture list
/// </summary>
void RenderPipeline::loadOneTexture(std::string textureName)
{
	TextureData td;

	GLint mode = GL_RGB;

	std::string texturePath = getAssetPrefixPath(ASSET_TYPE::TEXTURE) + textureName + TEXTURE_EXTENSION_CONST;
	SDL_Surface *image_p = IMG_Load(texturePath.c_str()); //TODO move to an internal helper function

	if (image_p->format->BytesPerPixel == 4)
		mode = GL_RGBA; 

	GLuint glTexId;

	glGenTextures(1, &glTexId);
	glBindTexture(GL_TEXTURE_2D, glTexId);

	glTexImage2D(GL_TEXTURE_2D, 0, mode, image_p->w, image_p->h, 0, mode, GL_UNSIGNED_BYTE, image_p->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	td.texID = glTexId;

	SDL_FreeSurface(image_p);
	image_p = nullptr;

	_textures_p->emplace(textureName, td);

}

/// <summary>
/// Helper method for load screen
/// Draws a simple green screen
/// </summary>
void RenderPipeline::drawLoadScreen()
{
	if (!haveContext())
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _renderWidth, _renderHeight);

	glClearColor(0.1f, 0.75f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/// <summary>
/// Helper method for unload screen
/// Draws a simple blue screen
/// </summary>
void RenderPipeline::drawUnloadScreen()
{
	if (!haveContext())
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _renderWidth, _renderHeight);

	glClearColor(0.1f, 0.25f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

/// <summary>
/// Helper method for idle screen
/// Draws a simple red screen
/// </summary>
void RenderPipeline::drawIdleScreen()
{
	if (!haveContext())
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _renderWidth, _renderHeight);

	glClearColor(0.75f, 0.5f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/// <summary>
/// Renders a frame
/// </summary>
void RenderPipeline::doRender(RenderableScene *scene, RenderableOverlay *overlay)
{

	//used for animation and basically nothing else
	_frameCount++;

	//will remain in final
	if (scene == nullptr)
	{
		drawNullScene();
	}
	else
	{
		drawCamera(scene); //set up the camera

		if (_deferredStageEnabled)
		{
			drawObjects(scene); //do the geometry pass
			drawShadows(scene); //do the shadow map
			drawLighting(scene); //do the lighting pass
		}
		
		if (_forwardStageEnabled)
		{ 
			//TODO something something depth buffer
			drawForward(scene); //do the forward pass
		}
		
		if (_postprocessingEnabled)
			drawPostProcessing(scene); //do the postprocessing
		else
			drawPostBypass(scene);
	}

	if(_overlayStageEnabled && overlay != nullptr)
	{
		drawOverlay(overlay); //do the overlay pass
	}
}

/// <summary>
/// Draws a blank scene
/// Used when no scene data is available
/// </summary>
void RenderPipeline::drawNullScene()
{
	//fallback drawing routine if no scene is available
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int w, h;
	SDL_GL_GetDrawableSize(_window_p, &w, &h);
	glViewport(0, 0, w, h);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

/// <summary>
/// "Draws" the camera
/// Actually sets up base matrices because in OpenGL you move the whole world
/// </summary>
void RenderPipeline::drawCamera(RenderableScene *scene) //TODO rename?
{
	RenderableCamera *camera = &scene->camera;

	//"draw" the camera, actually just set up base matrices

	glm::mat4 projection = glm::perspective(camera->viewAngle, (float)_renderWidth / (float)_renderHeight, camera->nearPlane, camera->farPlane);
	glm::mat4 look = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	glm::mat4 translation = glm::translate(look, camera->position * -1.0f);
	glm::mat4 rotation = glm::mat4();
	rotation = glm::rotate(rotation, camera->rotation.z, glm::vec3(0, 0, 1));
	rotation = glm::rotate(rotation, camera->rotation.x, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, camera->rotation.y, glm::vec3(0, 1, 0));
	glm::mat4 view = rotation * translation;
	_baseModelViewMatrix = view;
	_baseModelViewProjectionMatrix = projection * view;
	_baseProjectionMatrix = projection;

	_cameraRightWorld = glm::vec3(view[0][0], view[1][0], view[2][0]);
	_cameraUpWorld = glm::vec3(view[0][1], view[1][1], view[2][1]);

	//also get main directional and ambient lighting here since multiple pipeline stages need it
	bool foundMainDirectionalLight = false;
	for (int eachLight = 0; eachLight < scene->lights.size(); eachLight++)
	{
		if (scene->lights[eachLight].type == RenderableLightType::DIRECTIONAL)
		{
			_mainDirectionalLight = (scene->lights[eachLight]);
			//SDL_Log("Found a directional light!");
			foundMainDirectionalLight = true;
			break;
		}
	}

	//fallback: black directional light
	if (!foundMainDirectionalLight)
	{
		RenderableLight mdl;
		mdl.intensity = 0.0f;
		_mainDirectionalLight = mdl;
	}

	//and the ambient
	_allAmbientLight = computeAmbientLight(scene);

}

/// <summary>
/// Draws all objects in the scene
/// Does some setup, then calls drawObject for each object
/// Essentially the geometry pass of a deferred renderer
/// </summary>
void RenderPipeline::drawObjects(RenderableScene *scene)
{

	//bind framebuffer and clear
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferData.fbo);
	glViewport(0, 0, _renderWidth, _renderHeight);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw objects 
	for (int i = 0; i < scene->objects.size(); i++)
	{
		RenderableObject *ro = &scene->objects[i];
		drawObject(ro);
	}

}

/// <summary>
/// Draws a single object
/// Relys on some setup done in drawObjects(), so only call from there
/// Will fail to draw elegantly if resources are missing
/// </summary>
void RenderPipeline::drawObject(RenderableObject *object)
{
	//draw one arbitraty object
	//NOTE: should always be tolerant of missing resources!

	//set shader program
	glUseProgram(_geometryPassData.program);

	//check if a model exists
	bool hasModel = false;
	bool hasATexture = false;
	bool hasNTexture = false;
	ModelData modelData;
	TextureData texData;

	if (_models_p->count(object->modelName) > 0)
		hasModel = true;
	//else
	//	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: model is missing!");

	if (_textures_p->count(object->albedoName) > 0)
		hasATexture = true;
	//else
	//	SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: texture is missing!");

	if (!(object->normalName.empty()) && _textures_p->count(object->normalName) > 0)
		hasNTexture = true;

	//try to bind model
	if (hasModel)
	{
		modelData = _models_p->find(object->modelName)->second;
		if (modelData.vaoID != 0)
		{
			glBindVertexArray(modelData.vaoID);
		}
		else
		{
			hasModel = false;
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: model has no VAO!");
		}

	}

	if (!hasModel)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: object has no model!");
		return;
	}


	//try to bind texture
	if (hasATexture)
	{
		texData = _textures_p->find(object->albedoName)->second;
		if (texData.texID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texData.texID);
			glUniform1i(_geometryPassData.programTexture, 0);
		}
		else
		{
			hasATexture = false;
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: texture has no texID!");
		}
	}

	if (!hasATexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fallbackTextureID);
		glUniform1i(_geometryPassData.programTexture, 0);
	}

	//try to bind normal map
	if (hasNTexture)
	{
		texData = _textures_p->find(object->normalName)->second;
		if (texData.texID != 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texData.texID);
			glUniform1i(_geometryPassData.programNormal, 1);
			glUniform1i(_geometryPassData.programHasNorm, GL_TRUE);
		}
		else
		{
			hasNTexture = false;
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: texture has no texID!");
		}
	}

	if (!hasNTexture)
	{
		glUniform1i(_geometryPassData.programHasNorm, GL_FALSE);
	}

	glUniform1f(_geometryPassData.programSmoothness, object->smoothness);

	//transform!
	glm::mat4 objectMVM = glm::mat4();
	objectMVM = glm::translate(objectMVM, object->position);
	//SDL_Log("%f, %f, %f", object->position.x, object->position.y, object->position.z);	
	objectMVM = glm::rotate(objectMVM, object->rotation.z, glm::vec3(0, 0, 1));
	objectMVM = glm::rotate(objectMVM, object->rotation.x, glm::vec3(1, 0, 0));
	objectMVM = glm::rotate(objectMVM, object->rotation.y, glm::vec3(0, 1, 0));
	objectMVM = glm::scale(objectMVM, object->scale);
	glm::mat4 objectMVPM = _baseModelViewProjectionMatrix * objectMVM;
	//glm::mat4 objectMVM2 = _baseModelViewMatrix * objectMVM;
	glUniformMatrix4fv(_geometryPassData.programMVPM, 1, GL_FALSE, &objectMVPM[0][0]);
	glUniformMatrix4fv(_geometryPassData.programMM, 1, GL_FALSE, &objectMVM[0][0]);

	//draw!
	if (hasModel)
	{
		glDrawArrays(GL_TRIANGLES, 0, modelData.numVerts);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}



	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// <summary>
/// prepares the shadow map
/// </summary>
void RenderPipeline::drawShadows(RenderableScene *scene)
{

	//build base matrix
	glm::mat4 projection = glm::ortho<float>(-GlobalPrefs::rShadowMapSide, GlobalPrefs::rShadowMapSide, -GlobalPrefs::rShadowMapSide, GlobalPrefs::rShadowMapSide, -GlobalPrefs::rShadowMapNear, GlobalPrefs::rShadowMapFar);
	glm::mat4 look = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	glm::vec3 correctedPosition = glm::vec3(scene->camera.position.x, _mainDirectionalLight.position.y, scene->camera.position.z);
	glm::mat4 translation = glm::translate(look, correctedPosition * -1.0f); //  
	glm::mat4 rotation = glm::mat4();
	rotation = glm::rotate(rotation, _mainDirectionalLight.rotation.z, glm::vec3(0, 0, 1));
	rotation = glm::rotate(rotation, _mainDirectionalLight.rotation.x, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, _mainDirectionalLight.rotation.y, glm::vec3(0, 1, 0));
	glm::mat4 view = rotation * translation;
	_depthModelViewMatrix = view;
	_depthModelViewProjectionMatrix = projection * view;

	//bind framebuffer and program
	glUseProgram(_shadowPassData.program);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowPassData.fbo);
	glViewport(0, 0, GlobalPrefs::rShadowMapSize, GlobalPrefs::rShadowMapSize);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glClear(GL_DEPTH_BUFFER_BIT);

	//draw objects into shadow map
	for (int i = 0; i < scene->objects.size(); i++)
	{
		RenderableObject *object = &scene->objects[i];

		//check if a model exists
		bool hasModel = false;
		ModelData modelData;

		if (_models_p->count(object->modelName) > 0)
			hasModel = true;

		//try to bind model
		if (hasModel)
		{
			modelData = _models_p->find(object->modelName)->second;
			if (modelData.vaoID != 0)
			{
				glBindVertexArray(modelData.vaoID);
			}
			else
			{
				hasModel = false;
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: model has no VAO!");
			}

		}

		if (!hasModel)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: object has no model!");
			break;
		}

		//transform!
		glm::mat4 objectMVM = glm::mat4();
		objectMVM = glm::translate(objectMVM, object->position);
		//SDL_Log("%f, %f, %f", object->position.x, object->position.y, object->position.z);
		objectMVM = glm::scale(objectMVM, object->scale);
		objectMVM = glm::rotate(objectMVM, object->rotation.y, glm::vec3(0, 1, 0));
		objectMVM = glm::rotate(objectMVM, object->rotation.x, glm::vec3(1, 0, 0));
		objectMVM = glm::rotate(objectMVM, object->rotation.z, glm::vec3(0, 0, 1));
		glm::mat4 objectMVPM = _depthModelViewProjectionMatrix * objectMVM;
		glUniformMatrix4fv(_shadowPassData.programMVPM, 1, GL_FALSE, &objectMVPM[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, modelData.numVerts);

		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);

}

/// <summary>
/// Draws all lights in the scene
/// May be broken up with helper methods later
/// This is the lighting pass of a deferred renderer
/// </summary>
void RenderPipeline::drawLighting(RenderableScene *scene)
{

	//setup framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _postProcessingData.fbo);
	glViewport(0, 0, _renderWidth, _renderHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable blending for lights
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);

	//draw main scene/lighting pass (ambient and main directional)
	drawLightingMainPass(scene);

	for (auto it = scene->lights.begin(); it < scene->lights.end(); it++)
	{
		switch (it->type)
		{
		case RenderableLightType::POINT:
			drawLightingPointLight(*it, scene);
			break;
		case RenderableLightType::SPOT:
			drawLightingSpotLight(*it, scene);
			break;
		}
	}

	glDisable(GL_BLEND);

	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/// <summary>
/// Draws the main lighting pass
/// Includes ambient component and main directional light, with shadows
/// </summary>
void RenderPipeline::drawLightingMainPass(RenderableScene *scene) //will need more args
{
	//bind shader
	glUseProgram(_lightingPassData.program);

	//bind buffers
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture0);
	//glBindTexture(GL_TEXTURE_2D, _shadowFramebufferDepthID);
	glUniform1i(_lightingPassData.programTexture0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture1);
	glUniform1i(_lightingPassData.programTexture1, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture2);
	glUniform1i(_lightingPassData.programTexture2, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.depth);
	glUniform1i(_lightingPassData.programTexture3, 3);

	//bind shadow mapping buffer
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _shadowPassData.texture);
	glUniform1i(_lightingPassData.programTextureS, 4);

	//calculate and bind shadow bias matrix
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	glm::mat4 depthBiasMVP = biasMatrix * _depthModelViewProjectionMatrix;
	//glm::mat4 depthBiasMVP = glm::inverse(_depthModelViewProjectionMatrix);
	glUniformMatrix4fv(_lightingPassData.programBiasM, 1, GL_FALSE, &depthBiasMVP[0][0]);

	//bind clear color		
	glm::vec3 clear = scene->camera.clearColor;
	glUniform3f(_lightingPassData.programColor, clear.r, clear.g, clear.b);

	//bind ambient light
	glm::vec3 ambient = _allAmbientLight;
	glUniform3f(_lightingPassData.programAmbient, ambient.r, ambient.g, ambient.b);

	//bind directional light color
	glm::vec3 directional = _mainDirectionalLight.color * _mainDirectionalLight.intensity;
	glUniform3f(_lightingPassData.programDirColor, directional.r, directional.g, directional.b);

	//bind directional light facing
	glm::mat4 rotMatrix = glm::mat4();
	rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.y, glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.x, glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.z, glm::vec3(0, 0, 1));
	glm::vec3 lightFacing = rotMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glUniform3f(_lightingPassData.programDirFacing, lightFacing.x, lightFacing.y, lightFacing.z);

	//bind camera position
	glm::vec3 cPos = scene->camera.position;
	glUniform3f(_lightingPassData.programCameraPos, cPos.x, cPos.y, cPos.z);

	//setup vertices
	glBindVertexArray(_fullscreenQuadData.vao);

	//draw
	glDrawArrays(GL_TRIANGLES, 0, _fullscreenQuadData.vertices);

	//unbind
	glBindVertexArray(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// <summary>
/// Draws a single point light in a lighting pass (using fullscreen quad for now)
/// </summary>
void RenderPipeline::drawLightingPointLight(RenderableLight light, RenderableScene *scene)
{
	glUseProgram(_pointlightPassData.program);

	//bind buffers
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture0);
	glUniform1i(_pointlightPassData.programTexture0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture1);
	glUniform1i(_pointlightPassData.programTexture1, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture2);
	glUniform1i(_pointlightPassData.programTexture2, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.depth);
	glUniform1i(_pointlightPassData.programTexture3, 3);

	//bind camera and lighting data
	glm::vec3 cPos = scene->camera.position;
	glUniform3f(_pointlightPassData.programCameraPos, cPos.x, cPos.y, cPos.z);
	glm::vec3 lPos = light.position;
	glUniform3f(_pointlightPassData.programLightPos, lPos.x, lPos.y, lPos.z);
	glm::vec3 lColor = light.color;
	glUniform3f(_pointlightPassData.programLightColor, lColor.x, lColor.y, lColor.z);
	glUniform1f(_pointlightPassData.programLightIntensity, light.intensity);
	glUniform1f(_pointlightPassData.programLightRange, light.range);

	//setup vertices
	glBindVertexArray(_fullscreenQuadData.vao);

	//draw
	glDrawArrays(GL_TRIANGLES, 0, _fullscreenQuadData.vertices);

	//unbind
	glBindVertexArray(0);
}

/// <summary>
/// Draws a single spot light in a lighting pass
/// </summary>
void RenderPipeline::drawLightingSpotLight(RenderableLight light, RenderableScene *scene)
{
	//prepare direction vector for "rotation"
	glm::mat4 rotMatrix = glm::mat4();
	rotMatrix = glm::rotate(rotMatrix, light.rotation.y, glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, light.rotation.x, glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, light.rotation.z, glm::vec3(0, 0, 1));
	glm::vec3 lightFacing = rotMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	glUseProgram(_spotlightPassData.program);

	//bind buffers
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture0);
	glUniform1i(_spotlightPassData.programTexture0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture1);
	glUniform1i(_spotlightPassData.programTexture1, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.texture2);
	glUniform1i(_spotlightPassData.programTexture2, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.depth);
	glUniform1i(_spotlightPassData.programTexture3, 3);

	//bind camera and lighting data
	glm::vec3 cPos = scene->camera.position;
	glUniform3f(_spotlightPassData.programCameraPos, cPos.x, cPos.y, cPos.z);

	glm::vec3 lPos = light.position;
	glUniform3f(_spotlightPassData.programLightPos, lPos.x, lPos.y, lPos.z);

	glUniform3f(_spotlightPassData.programLightDir, lightFacing.x, lightFacing.y, lightFacing.z);

	glm::vec3 lColor = light.color;
	glUniform3f(_spotlightPassData.programLightColor, lColor.x, lColor.y, lColor.z);
	glUniform1f(_spotlightPassData.programLightIntensity, light.intensity);
	glUniform1f(_spotlightPassData.programLightRange, light.range);
	glUniform1f(_spotlightPassData.programLightAngle, light.angle);

	//setup vertices
	glBindVertexArray(_fullscreenQuadData.vao);

	//draw
	glDrawArrays(GL_TRIANGLES, 0, _fullscreenQuadData.vertices);

	//unbind
	glBindVertexArray(0);

}

/// <summary>
/// Helper method for scene lighting
/// Computes ambient light values
/// </summary>
glm::vec3 RenderPipeline::computeAmbientLight(RenderableScene *scene)
{
	glm::vec3 totalAmbientLight = glm::vec3(0);

	std::vector<RenderableLight> *lights = &scene->lights;
	for (const RenderableLight& light : *lights)
	{
		if (light.type == RenderableLightType::AMBIENT)
		{
			totalAmbientLight += (light.color * light.intensity);
		}
	}

	return totalAmbientLight;
}

/// <summary>
/// Draws all forward (including billboard) objects in the scene
/// 
/// </summary>
void RenderPipeline::drawForward(RenderableScene *scene)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _postProcessingData.fbo);
	glViewport(0, 0, _renderWidth, _renderHeight);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//bind program and vars
	glUseProgram(_forwardPassData.program);

	glm::vec3 ambient = _allAmbientLight;
	glUniform3f(_forwardPassData.programAmbient, ambient.r, ambient.g, ambient.b);

	glm::vec3 directional = _mainDirectionalLight.color * _mainDirectionalLight.intensity;
	glUniform3f(_forwardPassData.programDLight, directional.r, directional.g, directional.b);

	glm::mat4 rotMatrix = glm::mat4();
	rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.z, glm::vec3(0, 0, 1));
	rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.x, glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.y, glm::vec3(0, 1, 0));
	glm::vec3 lightFacing = rotMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glUniform3f(_forwardPassData.programDLightFacing, lightFacing.x, lightFacing.y, lightFacing.z);

	glm::vec3 cPos = scene->camera.position;
	glUniform3f(_forwardPassData.programCameraPos, cPos.x, cPos.y, cPos.z);

	glUniformMatrix4fv(_forwardPassData.programPM, 1, GL_FALSE, &_baseProjectionMatrix[0][0]);

	for (auto &obj : scene->forwardObjects)
	{
		drawForwardObject(&obj);
	}

}

/// <summary>
/// Draws a single forward object
/// 
/// </summary>
void RenderPipeline::drawForwardObject(RenderableObject * object)
{
	//check and set animated and offsets
	//set animation
	if (object->frameCount <= 1)
		glUniform1i(_forwardPassData.programAnimated, GL_FALSE);
	else
	{
		glUniform1i(_forwardPassData.programAnimated, GL_TRUE);
		glm::vec4 offsets = computeAnimationOffsets(*object);
		glUniform4fv(_forwardPassData.programOffsets, 1, &offsets[0]);
	}

	//check if a model exists
	bool hasModel = false;
	bool hasATexture = false;
	ModelData modelData;
	TextureData texData;

	if (_models_p->count(object->modelName) > 0)
		hasModel = true;

	if (_textures_p->count(object->albedoName) > 0)
		hasATexture = true;

	//try to bind model
	if (hasModel)
	{
		modelData = _models_p->find(object->modelName)->second;
		if (modelData.vaoID != 0)
		{
			glBindVertexArray(modelData.vaoID);
		}
		else
		{
			hasModel = false;
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: model has no VAO!");
		}

	}

	if (!hasModel)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: object has no model!");
		return;
	}


	//try to bind texture
	if (hasATexture)
	{
		texData = _textures_p->find(object->albedoName)->second;
		if (texData.texID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texData.texID);
			glUniform1i(_forwardPassData.programTexture, 0);
		}
		else
		{
			hasATexture = false;
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: texture has no texID!");
		}
	}

	if (!hasATexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fallbackTextureID);
		glUniform1i(_forwardPassData.programTexture, 0);
	}

	glUniform1f(_forwardPassData.programSmoothness, object->smoothness);

	//normal or billboard transform
	if (object->type == RenderableType::BILLBOARD)
	{
		// billboard matrices
		glm::mat4 objectMVM = glm::mat4();
		objectMVM = glm::translate(objectMVM, object->position);
		objectMVM = glm::scale(objectMVM, object->scale);
		objectMVM = glm::rotate(objectMVM, object->rotation.z, glm::vec3(0, 0, 1));
		objectMVM = glm::rotate(objectMVM, object->rotation.x, glm::vec3(1, 0, 0));
		objectMVM = glm::rotate(objectMVM, object->rotation.y, glm::vec3(0, 1, 0));
		glm::mat4 objectMVM2 = _baseModelViewMatrix * objectMVM;

		//reset part to identity
		objectMVM2[0][0] = 1;
		objectMVM2[0][1] = 0;
		objectMVM2[0][2] = 0;
		objectMVM2[1][0] = 0;
		objectMVM2[1][1] = 1;
		objectMVM2[1][2] = 0;
		objectMVM2[2][0] = 0;
		objectMVM2[2][1] = 0;
		objectMVM2[2][2] = 1;

		//rescale
		objectMVM2 = glm::scale(objectMVM2, object->scale);

		glUniformMatrix4fv(_forwardPassData.programMVM, 1, GL_FALSE, &objectMVM2[0][0]);
		glUniformMatrix4fv(_forwardPassData.programMM, 1, GL_FALSE, &objectMVM[0][0]);

		glDepthMask(GL_FALSE);
		glUniform1i(_forwardPassData.programBillboard, GL_TRUE);
	}
	else
	{	
		glm::mat4 objectMVM = glm::mat4();
		objectMVM = glm::translate(objectMVM, object->position);
		objectMVM = glm::rotate(objectMVM, object->rotation.z, glm::vec3(0, 0, 1));
		objectMVM = glm::rotate(objectMVM, object->rotation.x, glm::vec3(1, 0, 0));
		objectMVM = glm::rotate(objectMVM, object->rotation.y, glm::vec3(0, 1, 0));
		objectMVM = glm::scale(objectMVM, object->scale);
		glm::mat4 objectMVM2 = _baseModelViewMatrix * objectMVM;
		glUniformMatrix4fv(_forwardPassData.programMVM, 1, GL_FALSE, &objectMVM2[0][0]);
		glUniformMatrix4fv(_forwardPassData.programMM, 1, GL_FALSE, &objectMVM[0][0]);

		glDepthMask(GL_TRUE);
		glUniform1i(_forwardPassData.programBillboard, GL_FALSE);
	}

	//draw!
	if (hasModel)
	{
		glDrawArrays(GL_TRIANGLES, 0, modelData.numVerts);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// Apply "null" postprocessing: just blit the buffer
/// </summary>
void RenderPipeline::drawPostBypass(RenderableScene *scene)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, _postFramebufferID);

	int w, h;
	SDL_GL_GetDrawableSize(_window_p, &w, &h);
	glViewport(0, 0, w, h);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_postBypassData.program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _postProcessingData.texture);
	glUniform1i(_postBypassData.programTexture, 0);

	glBindVertexArray(_fullscreenQuadData.vao);
	glDrawArrays(GL_TRIANGLES, 0, _fullscreenQuadData.vertices);

	glBindVertexArray(0);
}

/// <summary>
/// Applies postprocessing and blits buffers
/// May be broken up with helper methods later
/// </summary>
void RenderPipeline::drawPostProcessing(RenderableScene *scene)
{
	//left it like this because we may want to override later
	const float blurFactor = GlobalPrefs::rBlurFactor;
	const float blurAmount = GlobalPrefs::rBlurAmount;
	const float dofFactor = GlobalPrefs::rDofFactor;
	const float dofAmount = GlobalPrefs::rDofAmount;
	const float fogFactor = GlobalPrefs::rFogFactor;
	const float fogAmount = GlobalPrefs::rFogAmount;
	const glm::vec3 fogColor = glm::vec3(1.0f, 1.0f, 1.0f);

	//draw postprocessing
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, _postFramebufferID);

	int w, h;
	SDL_GL_GetDrawableSize(_window_p, &w, &h);
	glViewport(0, 0, w, h);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_postProcessingData.program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _postProcessingData.texture);
	glUniform1i(_postProcessingData.programTexture, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _postProcessingData.smearTexture);
	glUniform1i(_postProcessingData.programSmearTexture, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _framebufferData.depth);
	glUniform1i(_postProcessingData.programDepthTexture, 2);

	glUniform1f(_postProcessingData.programBlurAmount, blurAmount);
	glUniform1f(_postProcessingData.programDofAmount, dofAmount);
	glUniform1f(_postProcessingData.programDofFactor, dofFactor);
	glUniform1f(_postProcessingData.programFogAmount, fogAmount);
	glUniform1f(_postProcessingData.programFogFactor, fogFactor);
	glUniform3f(_postProcessingData.programFogColor, fogColor.r, fogColor.g, fogColor.b);

	glBindVertexArray(_fullscreenQuadData.vao);
	glDrawArrays(GL_TRIANGLES, 0, _fullscreenQuadData.vertices);

	glBindVertexArray(0);

	drawPostProcessingCopySmearbuffer(blurFactor, blurAmount);

}

void RenderPipeline::drawPostProcessingCopySmearbuffer(float blurFactor, float blurAmount)
{
	//copy smearbuffer into (temporary) texture
	GLuint sBufferTexture = 0;
	glGenTextures(1, &sBufferTexture);
	glBindTexture(GL_TEXTURE_2D, sBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _renderWidth, _renderHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _postProcessingData.smearFbo);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 0, 0, _renderWidth, _renderHeight, 0);

	//blend into smearbuffer with shader
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _postProcessingData.smearFbo);
	//glBlitFramebuffer(0, 0, _renderWidth, _renderHeight, 0, 0, _renderWidth, _renderHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glViewport(0, 0, _renderWidth, _renderHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_postProcessingData.copyProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _postProcessingData.texture);
	glUniform1i(_postProcessingData.copyProgramLastTexture, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sBufferTexture);
	glUniform1i(_postProcessingData.copyProgramSmearTexture, 1);

	glUniform1f(_postProcessingData.copyProgramFactor, blurFactor);
	glUniform1f(_postProcessingData.copyProgramBlurAmount, blurAmount);

	glBindVertexArray(_fullscreenQuadData.vao);
	glDrawArrays(GL_TRIANGLES, 0, _fullscreenQuadData.vertices);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//delete temporary texture
	glDeleteTextures(1, &sBufferTexture);
}

/// <summary>
/// Draws the overlay
/// 
/// </summary>
void RenderPipeline::drawOverlay(RenderableOverlay *overlay)
{
	//draw overlay

	//need to make sure right framebuffer is set, clear depth but not color
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _renderWidth, _renderHeight);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	//bind program, vertex array, some matrix stuff
	glBindVertexArray(_overlayDrawData.vao);
	glUseProgram(_overlayDrawData.program);

	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//turns out we do have to sort elements
	std::sort(overlay->elements.begin(), overlay->elements.end(), [](RenderableObject a, RenderableObject b) {return a.position.z < b.position.z; }); //congratulations you found the JS programmer

	//actually draw the stuff
	for (RenderableObject &el : overlay->elements)
	{
		drawOverlayElement(&el);
		//glClear(GL_DEPTH_BUFFER_BIT); //a hack because z-indexing was not used
	}

	glDisable(GL_BLEND);
	glBindVertexArray(0);

}

/// <summary>
/// Draws a single overlay element
/// </summary>
void RenderPipeline::drawOverlayElement(RenderableObject * element)
{
	//get texture
	GLuint texture = _fallbackTextureID;
	auto texEl = _textures_p->find(element->albedoName);
	if (texEl != _textures_p->end())
	{
		auto texData = texEl->second;
		if (texData.texID != 0)
		{
			texture = texData.texID;
		}
	}	

	//compute matrix
	glm::mat4 objectMVM = glm::mat4();
	objectMVM = glm::translate(objectMVM, element->position);
	objectMVM = glm::scale(objectMVM, element->scale);
	objectMVM = glm::rotate(objectMVM, element->rotation.z, glm::vec3(0, 0, 1));
	glm::mat4 objectMVPM = _overlayDrawData.MVPM * objectMVM;

	//set animation
	if (element->frameCount <= 1)
		glUniform1i(_overlayDrawData.programAnimated, GL_FALSE);
	else
	{
		glUniform1i(_overlayDrawData.programAnimated, GL_TRUE);
		glm::vec4 offsets = computeAnimationOffsets(*element);
		glUniform4fv(_overlayDrawData.programOffsets, 1, &offsets[0]);
	}

	//bind texture, bind matrix, draw
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(_overlayDrawData.programTexture, 0);
	glUniformMatrix4fv(_overlayDrawData.programMVPM, 1, GL_FALSE, &objectMVPM[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

/// <summary>
/// Helper function for animations
/// Computes offsets for animated objects
/// </summary>
glm::vec4 RenderPipeline::computeAnimationOffsets(const RenderableObject & object)
{
	//SDL_Log("%s : %i", object.albedoName, object.currentFrame);

	//crudely adapted from existing code
	int frames = object.frameCount;
	int animationCount = (int)(_frameCount - object.startFrame) / object.frameDelay;

	//clamp if necessary
	if (object.animateOnce && animationCount >= object.frameCount-1)
	{
		//SDL_Log("%s : %i", object.albedoName.c_str(), animationCount);
		animationCount = object.frameCount-2; //there is at least one off-by-one error in the old code
	}
		

	if (frames == 0) {
		frames = 1;
	}

	int numberOfRows = 1;
	float offset;
	float yoffset = 0.0f;
	int xframes = frames;

	float fheight = 1.0f;
	int currentRow = 1;

	int tempacount = animationCount;

	if (frames <= 20) {
		offset = (float)(animationCount % frames) + 1.0f;
	}
	else {
		xframes = 20;
		numberOfRows = (int)(ceil(frames / 20.0f));
		if (animationCount == 0) {
			currentRow = 1;
		}
		else {
			if (animationCount > frames) {
				tempacount = animationCount % frames;
			}
			currentRow = (int)(ceil(tempacount / 20.0f));
		}

		// default
		offset = (float)(tempacount % 20) - 1;

		if ((tempacount == 0)) {
			offset = 0;
		}

		if (((tempacount % 20) == 0) && (tempacount != 0)) {
			offset = 19;
		}

		// 0 to 5
		if (currentRow != 0) {
			yoffset = (float)(currentRow - 1);// ((currentRow + numberOfRows - 1) % numberOfRows);
		}
		else {
			yoffset = 0.0f;
		}
		fheight = (float)(1.0f / ((float)(numberOfRows)));
	}

	float finalYOffset = (1.0f / (float)numberOfRows) * yoffset;

	/*
	if (frames > 20 && animationCount <= 30) {
		std::ostringstream ss;
		ss << finalYOffset << "\t" << yoffset << "\t" << animationCount << "\t" << tempacount << "\t" << currentRow << "\t" << numberOfRows << "\t" << offset;
		SDL_Log(ss.str().c_str());
	}
	*/

	return glm::vec4((1.0f / (float)xframes) * offset, finalYOffset, 1.0f / (float)xframes, (float)fheight);
}

/// <summary>
/// Draws the null overlay
/// Run if no overlay data is available, literally does nothing
/// </summary>
void RenderPipeline::drawNullOverlay()
{
	//probably just leave it blank yeah
}

/// <summary>
/// Helper method for context
/// Attempts to acquire OpenGL context, returns true on success
/// </summary>
bool RenderPipeline::acquireContext()
{
	return true;
}

/// <summary>
/// Helper method for context
/// Releases OpenGL context so others can use it
/// </summary>
bool RenderPipeline::releaseContext()
{
	//does nothing but we may need it later
	return true;
}

/// <summary>
/// Helper method for context
/// Checks if the renderer has OpenGL context
/// </summary>
bool RenderPipeline::haveContext()
{
	return true;
}

/// <summary>
/// Helper method for animation
/// Gets the current frame count
/// </summary>
int_least64_t RenderPipeline::currentFrame()
{
	return _frameCount;
}

/// <summary>
/// Setter method for deferred stage
/// </summary>
void RenderPipeline::setDeferredStage(bool enabled)
{
	_deferredStageEnabled = enabled;
}

/// <summary>
/// Setter method for forward stage
/// </summary>
void RenderPipeline::setForwardStage(bool enabled)
{
	_forwardStageEnabled = enabled;
}

/// <summary>
/// Setter method for overlay stage
/// </summary>
void RenderPipeline::setOverlayStage(bool enabled)
{
	_overlayStageEnabled = enabled;
}

/// <summary>
/// Setter method for postprocessing stage
/// </summary>
void RenderPipeline::setPostprocessingStage(bool enabled)
{
	_postprocessingEnabled = enabled;
}
