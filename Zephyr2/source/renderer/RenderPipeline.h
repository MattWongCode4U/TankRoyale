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

	//shader stuff
	GLuint _programID = 0;
	GLuint _shaderModelMatrixID = 0;
	GLuint _shaderMVPMatrixID = 0;
	GLuint _shaderTextureID = 0;
	GLuint _shaderNormalID = 0;
	GLuint _shaderHasNormID = 0;
	GLuint _shaderSmoothnessID = 0;

	//framebuffer stuff
	int _renderWidth = 0;
	int _renderHeight = 0;

	//stage enablement
	bool _deferredStageEnabled = true;
	bool _forwardStageEnabled = true;
	bool _overlayStageEnabled = true;
	bool _postprocessingEnabled = true;

	//framebuffer textures
	GLuint _framebufferID = 0;
	GLuint _framebufferTexture0ID = 0;
	GLuint _framebufferTexture1ID = 0;
	GLuint _framebufferTexture2ID = 0;
	GLuint _framebufferDepthID = 0;

	//framebuffer (initial pass) program and uniforms
	GLuint _framebufferDrawProgramID = 0;
	GLuint _framebufferDrawVertexArrayID = 0;
	GLuint _framebufferDrawVertexBufferID = 0;
	GLuint _framebufferDrawTex0ID = 0;
	GLuint _framebufferDrawTex1ID = 0;
	GLuint _framebufferDrawTex2ID = 0;
	GLuint _framebufferDrawTex3ID = 0;
	GLuint _framebufferDrawTexSID = 0;
	GLuint _framebufferDrawColorID = 0;
	GLuint _framebufferDrawAmbientID = 0;
	GLuint _framebufferDrawDirColorID = 0;
	GLuint _framebufferDrawDirFacingID = 0;
	GLuint _framebufferDrawCameraPosID = 0;
	GLuint _framebufferDrawBiasID = 0;

	//shadow pass program and uniforms, texture ID
	GLuint _shadowPassProgramID = 0;
	GLuint _shadowPassModelMatrixID = 0;
	GLuint _shadowPassMVPMatrixID = 0;
	GLuint _shadowFramebufferID = 0;
	GLuint _shadowFramebufferDepthID = 0;

	//point light pass program and uniforms
	GLuint _plightPassProgramID;
	GLuint _plightPassTex0ID = 0;
	GLuint _plightPassTex1ID = 0;
	GLuint _plightPassTex2ID = 0;
	GLuint _plightPassTex3ID = 0;
	GLuint _plightPassCameraPosID = 0;
	GLuint _plightPassLightPosID = 0;
	GLuint _plightPassLightIntensityID = 0;
	GLuint _plightPassLightColorID = 0;
	GLuint _plightPassLightRangeID = 0;

	//spot light pass program and uniforms
	GLuint _slightPassProgramID;
	GLuint _slightPassTex0ID = 0;
	GLuint _slightPassTex1ID = 0;
	GLuint _slightPassTex2ID = 0;
	GLuint _slightPassTex3ID = 0;
	GLuint _slightPassCameraPosID = 0;
	GLuint _slightPassLightPosID = 0;
	GLuint _slightPassLightDirID = 0;
	GLuint _slightPassLightIntensityID = 0;
	GLuint _slightPassLightColorID = 0;
	GLuint _slightPassLightRangeID = 0;
	GLuint _slightPassLightAngleID = 0;

	//postprocessing program and buffers
	GLuint _postProgramID = 0;
	GLuint _postProgramTexID = 0;
	GLuint _postProgramSmearTexID = 0;
	GLuint _postProgramDepthTexID = 0;
	GLuint _postProgramBlurAmountID = 0;
	GLuint _postProgramDofAmountID = 0;
	GLuint _postProgramDofFactorID = 0;
	GLuint _postProgramFogAmountID = 0;
	GLuint _postProgramFogFactorID = 0;
	GLuint _postProgramFogColorID = 0;
	GLuint _postFramebufferID = 0;
	GLuint _postFramebufferTexID = 0;
	GLuint _postSmearbufferID = 0;
	GLuint _postSmearbufferTexID = 0;
	GLuint _postCopyProgramID = 0;
	GLuint _postCopyProgramFactorID = 0;
	GLuint _postCopyProgramBlurAmountID = 0;
	GLuint _postCopyProgramLastTexID = 0;
	GLuint _postCopyProgramSmearTexID = 0;

	//overlay draw program and buffers (TODO move all to structs)
	OverlaySetup _overlayDrawData;

	ForwardSetup _forwardDrawData;

	//the texture of shame
	GLuint _fallbackTextureID = 0;

	//base MVP, may structify
	glm::mat4 _baseModelViewMatrix;
	glm::mat4 _baseModelViewProjectionMatrix;
	glm::mat4 _baseProjectionMatrix;
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