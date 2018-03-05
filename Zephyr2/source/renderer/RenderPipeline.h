#pragma once
#include "RenderableTypes.h"
#include "RendererInternalTypes.h"
#include <glm.hpp>
#include <SDL.h>


class RenderPipeline
{
private:

	//context and window
	SDL_Window * _window_p;
	SDL_GLContext _context_p;

	//frame count
	uint_least64_t _frameCount;

	//resource lists
	std::map<std::string, ModelData> *_models_p;
	std::map<std::string, TextureData> *_textures_p;


	//framebuffer stuff
	int _renderWidth = 0;
	int _renderHeight = 0;

	//stage enablement
	bool _deferredStageEnabled = true;
	bool _forwardStageEnabled = true;
	bool _overlayStageEnabled = true;
	bool _postprocessingEnabled = true;

	//framebuffer textures
	FramebufferSetup _framebufferData;

	//fullscreen quad (used for buffer drawing)
	FullscreenQuadSetup _fullscreenQuadData;

	//geometry pass program and uniforms
	GeometryPassSetup _geometryPassData;	

	//shadow pass program and uniforms, texture ID
	ShadowPassSetup _shadowPassData;

	//main lighting pass program and uniforms
	LightingPassSetup _lightingPassData;

	//point light pass program and uniforms
	PointLightPassSetup _pointlightPassData;

	//spot light pass program and uniforms
	SpotLightPassSetup _spotlightPassData;

	//forward pass program and uniforms
	ForwardPassSetup _forwardPassData;

	//postprocessing program and buffers
	PostProcessingSetup _postProcessingData;

	//overlay draw program and buffers (TODO move all to structs)
	OverlaySetup _overlayDrawData;

	//the texture of shame
	GLuint _fallbackTextureID = 0;

	//base MVP, may structify
	glm::mat4 _baseModelViewMatrix;
	glm::mat4 _baseModelViewProjectionMatrix;
	glm::mat4 _baseProjectionMatrix;
	glm::vec3 _cameraUpWorld;
	glm::vec3 _cameraRightWorld;
	glm::mat4 _depthModelViewMatrix;
	glm::mat4 _depthModelViewProjectionMatrix;
	RenderableLight _mainDirectionalLight;
	glm::vec3 _allAmbientLight;

	void drawNullScene();
	void drawCamera(RenderableScene * scene);

	void drawObjects(RenderableScene * scene);
	void drawObject(RenderableObject * object);
	void drawShadows(RenderableScene * scene);
	void drawLighting(RenderableScene * scene);
	void drawLightingMainPass(RenderableScene * scene);
	void drawLightingPointLight(RenderableLight light, RenderableScene * scene);
	void drawLightingSpotLight(RenderableLight light, RenderableScene * scene);
	void drawPostProcessing(RenderableScene * scene);
	void drawPostProcessingCopySmearbuffer(float blurFactor, float blurAmount);

	void drawForward(RenderableScene * scene);
	void drawForwardObject(RenderableObject * object);

	void drawOverlay(RenderableOverlay * overlay);
	void drawNullOverlay();
	void drawOverlayElement(RenderableObject * element);

	glm::vec4 computeAnimationOffsets(const RenderableObject &object);
	glm::vec3 computeAmbientLight(RenderableScene * scene);

	void setupWindow();
	void setupProgram();
	void cleanupProgram();

	void setupFallbacks();
	void cleanupFallbacks();
	void setupFramebuffers();
	void cleanupFramebuffers();
	void setupFramebufferDraw();
	void cleanupFramebufferDraw();
	void setupShadowMapping();
	void cleanupShadowMapping();
	void setupPostProcessing();
	void cleanupPostProcessing();
	void setupForward();
	void cleanupForward();
	void setupOverlay();
	void cleanupOverlay();

public:
	RenderPipeline(SDL_Window *window_p);
	~RenderPipeline();

	void setupGLOnThread();
	void setupSceneOnThread();
	void cleanupSceneOnThread();
	void cleanupGLOnThread();
		
	void loadAllResources();
	void loadOneModel(std::string modelName);
	void loadOneTexture(std::string textureName);

	void drawLoadScreen();
	void drawUnloadScreen();
	void drawIdleScreen();
	void doRender(RenderableScene * scene, RenderableOverlay * overlay);

	bool acquireContext();
	bool releaseContext();
	bool haveContext();

	void setDeferredStage(bool enabled);
	void setForwardStage(bool enabled);
	void setOverlayStage(bool enabled);
	void setPostprocessingStage(bool enabled);

};