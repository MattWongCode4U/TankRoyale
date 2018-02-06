#include <glew.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtx\euler_angles.hpp>
#include <SDL.h>

#include <thread>
#include <queue>
#include <atomic>

#include "RenderEngine.h"

#include "GlobalPrefs.h"

#include "RenderableTypes.h"
#include "RendererInternalTypes.h"

#include "Shaders.h"
#include "Quad.h"
#include "Cube.h"

//CONSTANTS: will change at least some to config options

const int_least64_t IDLE_DELAY_CONST = 10;

/// <summary>
/// Actual RenderEngine implementation, using PIMPL pattern for a modicum of isolation
/// </summary>
class RenderEngineImplementation
{
public:

	/// <summary>
	/// Implementation constructor
	/// Does nothing
	/// (runs on game engine thread)
	/// </summary>
	RenderEngineImplementation()
	{
	}

	/// <summary>
	/// Implementation start method
	/// Creates message handlers, does some setup, and starts render thread
	/// (runs on game engine thread)
	/// </summary>
	void start()
	{

		//TODO window will be passed in from RenderSystem somehow
		//_window_p = g_window_p; //really ought to switch to dependency injection

		_state = RendererState::idle;
		
		//necessary protection on macOS
		_lastScene_p = nullptr;
		_lastOverlay_p = nullptr;

		//spawn thread
		_isRunning = true;
		_renderThread_p = new std::thread(&RenderEngineImplementation::loop, this);
	}

	/// <summary>
	/// Implementation update method
	/// Since RenderEngine is totally asynchronous, this does nothing
	/// (runs on game engine thread)
	/// </summary>
	void update()
	{
	}

	/// <summary>
	/// Implementation destructor
	/// Kills render thread, cleans up message handlers and queues
	/// (runs on game engine thread)
	/// </summary>
	~RenderEngineImplementation()
	{
		//destructor

		//kill render thread first
		_isRunning = false;
		_renderThread_p->join();
		delete(_renderThread_p);

		delete(_mqMutex_p);
		delete(_mr_p);
		delete(_mq_p);
		
		delete(_fmqMutex_p);
		delete(_fmr_p);
		delete(_fmq_p);

		delete(_imqMutex_p);
		delete(_imr_p);
		delete(_imq_p);
		
	}

private:

	//context and window
	SDL_Window *_window_p;
	SDL_GLContext _context_p;

	//state data
	std::atomic<RendererState> _state;
	RenderableScene *_lastScene_p;
	RenderableOverlay *_lastOverlay_p;

	//messaging stuff
	RenderMessageReceiver *_mr_p;
	RenderFileMessageReceiver *_fmr_p;
	RenderInputMessageReceiver *_imr_p;
	std::vector<std::shared_ptr<Message>> *_mq_p;
	std::vector<std::shared_ptr<Message>> *_fmq_p;
	std::vector<std::shared_ptr<Message>> *_imq_p;
	std::mutex *_mqMutex_p;
	std::mutex *_fmqMutex_p;
	std::mutex *_imqMutex_p;

	//resource lists
	std::map<std::string, ModelData> *_models_p;
	std::map<std::string, TextureData> *_textures_p;
	std::vector<ModelLoadingData> *_modelLoadQueue_p;
	std::vector<TextureLoadingData> *_textureLoadQueue_p;
	std::vector<ModelLoadingData> *_modelAwaitQueue_p;
	std::vector<TextureLoadingData> *_textureAwaitQueue_p;

	//threading stuff
	bool _isRunning;
	std::thread *_renderThread_p;

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

	//the texture of shame
	GLuint _fallbackTextureID = 0;

	//base MVP, may keep or remove
	
	glm::mat4 _baseModelViewMatrix;
	glm::mat4 _baseModelViewProjectionMatrix;
	glm::mat4 _depthModelViewMatrix;
	glm::mat4 _depthModelViewProjectionMatrix;
	RenderableLight _mainDirectionalLight;

	/// <summary>
	/// Threaded loop method
	/// Runs some setup, executes loop until signalled to stop, does some cleanup
	/// (runs on own thread and is actually the base of it)
	/// </summary>
	void loop()
	{
		//initial setup: run once

		SDL_Log("RenderEngine thread started!");

		setupStructuresOnThread();
		setupGLOnThread();
		setupSceneOnThread();

		//for testing
		_state = RendererState::idle;

		//loop: on RenderEngine thread
		while (_isRunning)
		{
			//doLoad, doRender/doImmediateLoad, doUnload

			checkQueue();

			switch ((RendererState)_state)
			{
			case RendererState::idle:
				//SDL_Log("Idle");
				doIdle();
				//std::this_thread::sleep_for(std::chrono::milliseconds(IDLE_DELAY_CONST)); //don't busywait!
				break;
			case RendererState::loading:
				//SDL_Log("Loading");
				doLoad();
				break;
			case RendererState::rendering:
				//SDL_Log("Rendering");
				doSingleLoad();
				doRender(); //this should run really absurdly fast
				break;
			case RendererState::unloading:
				//SDL_Log("Unloading");
				doUnload();
				break;
			}

			//std::this_thread::sleep_for(std::chrono::milliseconds(17));
			SDL_GL_SwapWindow(_window_p);
		}

		//force unload/release if we're not already unloaded
		if (_state != RendererState::idle)
		{
			startUnload();
			doUnload();
		}

		//cleanup after run
		cleanupSceneOnThread();
		cleanupGLOnThread();
		cleanupStructuresOnThread();

		SDL_Log("RenderEngine thread halted!");
	}

	/// <summary>
	/// Helper method for initial datastructure setup
	/// Creates model and texture resource list, load queues, and await queues
	/// </summary>
	void setupStructuresOnThread()
	{
		//setup data structures

		_models_p = new std::map<std::string, ModelData>();
		_textures_p = new std::map<std::string, TextureData>();
		_modelLoadQueue_p = new std::vector<ModelLoadingData>();
		_textureLoadQueue_p = new std::vector<TextureLoadingData>();
		_modelAwaitQueue_p = new std::vector<ModelLoadingData>();
		_textureAwaitQueue_p = new std::vector<TextureLoadingData>();
	}

	/// <summary>
	/// Helper method for initial OpenGL setup
	/// Creates OpenGL context and initializes GLEW
	/// </summary>
	void setupGLOnThread()
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
	void setupSceneOnThread()
	{
		setupWindow();
		setupProgram();
		setupFramebuffers();
		setupFramebufferDraw();
		setupShadowMapping();
		setupPostProcessing();
		setupFallbacks();
	}

	/// <summary>
	/// Helper method for final scene cleanup
	/// Confusingly named; actually handles window, shaders, G-buffer, and temporary cube
	/// </summary>
	void cleanupSceneOnThread()
	{
		cleanupProgram();
		cleanupFramebuffers();
		cleanupFramebufferDraw();
		cleanupShadowMapping();
		cleanupPostProcessing();
		cleanupFallbacks();
	}

	/// <summary>
	/// Helper method for final scene cleanup
	/// Confusingly named; actually handles window, shaders, G-buffer, and temporary cube
	/// </summary>
	void cleanupGLOnThread()
	{
		//TODO destroy the openGL context
	}

	/// <summary>
	/// Helper method for final datastructure cleanup
	/// Deletes all await and load queue, as well as resource lists
	/// </summary>
	void cleanupStructuresOnThread()
	{
		//delete data structures
		delete(_textureAwaitQueue_p);
		delete(_modelAwaitQueue_p);
		delete(_textureLoadQueue_p);
		delete(_modelLoadQueue_p);
		delete(_textures_p);
		delete(_models_p);
	}

	/// <summary>
	/// Helper method for initial setup
	/// Gets render width and height
	/// </summary>
	void setupWindow()
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
	void setupProgram()
	{
		_programID = Shaders::LoadShaders();
		_shaderModelMatrixID = glGetUniformLocation(_programID, "iModelMatrix");
		_shaderMVPMatrixID = glGetUniformLocation(_programID, "iModelViewProjectionMatrix");
		_shaderTextureID = glGetUniformLocation(_programID, "iTexImage");
		_shaderNormalID = glGetUniformLocation(_programID, "iNormImage");
		_shaderHasNormID = glGetUniformLocation(_programID, "iHasNorm");
		_shaderSmoothnessID = glGetUniformLocation(_programID, "iSmoothness");
	}

	/// <summary>
	/// Helper method for final cleanup
	/// Deletes shaders
	/// </summary>
	void cleanupProgram()
	{
		//delete shaders/program
		if (_programID > 0)
			glDeleteProgram(_programID);
	}

	/// <summary>
	/// Helper method initial setup
	/// Loads basic/placeholder texture
	/// </summary>
	void setupFallbacks()
	{
		//setup cube (fallback) texture

		std::string texturePath = TEXTURE_BASEPATH_CONST + "default" + TEXTURE_EXTENSION_CONST;

		SDL_Surface *image_p = FileHelper::loadImageFileFromStringRelative(texturePath);

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
	void cleanupFallbacks()
	{
		glDeleteTextures(1, &_fallbackTextureID);
	}

	/// <summary>
	/// Helper method initial setup
	/// Sets up FBO and textures for G-buffer
	/// </summary>
	void setupFramebuffers()
	{

		//gen FBO
		glGenFramebuffers(1, &_framebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _framebufferID);

		//gen framebuffer textures
		glGenTextures(1, &_framebufferTexture0ID);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture0ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _renderWidth, _renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _framebufferTexture0ID, 0);

		glGenTextures(1, &_framebufferTexture1ID);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture1ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _renderWidth, _renderHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _framebufferTexture1ID, 0);

		glGenTextures(1, &_framebufferTexture2ID);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture2ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8_SNORM, _renderWidth, _renderHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _framebufferTexture2ID, 0);

		//gen depthbuffer
		glGenTextures(1, &_framebufferDepthID);
		glBindTexture(GL_TEXTURE_2D, _framebufferDepthID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _renderWidth, _renderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _framebufferDepthID, 0);

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
	void cleanupFramebuffers()
	{
		//delete FBOs
		glDeleteTextures(1, &_framebufferTexture0ID);
		glDeleteTextures(1, &_framebufferTexture1ID);
		glDeleteTextures(1, &_framebufferTexture2ID);
		glDeleteTextures(1, &_framebufferDepthID);
		glDeleteFramebuffers(1, &_framebufferID);
	}

	/// <summary>
	/// Helper method initial setup
	/// Sets up fullscreen quad, shaders and uniform locations for lighting pass
	/// </summary>
	void setupFramebufferDraw()
	{
		//setup fullscreen quad VAO
		
		glGenVertexArrays(1, &_framebufferDrawVertexArrayID);
		glBindVertexArray(_framebufferDrawVertexArrayID);

		glGenBuffers(1, &_framebufferDrawVertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, _framebufferDrawVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//setup main pass shader
		_framebufferDrawProgramID = Shaders::LoadShadersFBDraw();
		_framebufferDrawTex0ID = glGetUniformLocation(_framebufferDrawProgramID, "fColor");
		_framebufferDrawTex1ID = glGetUniformLocation(_framebufferDrawProgramID, "fPosition");
		_framebufferDrawTex2ID = glGetUniformLocation(_framebufferDrawProgramID, "fNormal");
		_framebufferDrawTex3ID = glGetUniformLocation(_framebufferDrawProgramID, "fDepth");
		_framebufferDrawTexSID = glGetUniformLocation(_framebufferDrawProgramID, "sDepth");
		_framebufferDrawColorID = glGetUniformLocation(_framebufferDrawProgramID, "clearColor");
		_framebufferDrawAmbientID = glGetUniformLocation(_framebufferDrawProgramID, "ambientLight");
		_framebufferDrawDirColorID = glGetUniformLocation(_framebufferDrawProgramID, "dLightColor");
		_framebufferDrawDirFacingID = glGetUniformLocation(_framebufferDrawProgramID, "dLightFacing");;
		_framebufferDrawCameraPosID = glGetUniformLocation(_framebufferDrawProgramID, "cameraPos");;
		_framebufferDrawBiasID = glGetUniformLocation(_framebufferDrawProgramID, "biasMVP");

		//setup point pass shader
		_plightPassProgramID = Shaders::LoadShadersPointPass();
		_plightPassTex0ID = glGetUniformLocation(_plightPassProgramID, "fColor");
		_plightPassTex1ID = glGetUniformLocation(_plightPassProgramID, "fPosition");
		_plightPassTex2ID = glGetUniformLocation(_plightPassProgramID, "fNormal");
		_plightPassTex3ID = glGetUniformLocation(_plightPassProgramID, "fDepth");
		_plightPassCameraPosID = glGetUniformLocation(_plightPassProgramID, "cameraPos");
		_plightPassLightColorID = glGetUniformLocation(_plightPassProgramID, "lightColor");
		_plightPassLightPosID = glGetUniformLocation(_plightPassProgramID, "lightPos");
		_plightPassLightIntensityID = glGetUniformLocation(_plightPassProgramID, "lightIntensity");
		_plightPassLightRangeID = glGetUniformLocation(_plightPassProgramID, "lightRange");
		
		//setup spot pass shader
		_slightPassProgramID = Shaders::LoadShadersSpotPass();
		_slightPassTex0ID = glGetUniformLocation(_slightPassProgramID, "fColor");
		_slightPassTex1ID = glGetUniformLocation(_slightPassProgramID, "fPosition");
		_slightPassTex2ID = glGetUniformLocation(_slightPassProgramID, "fNormal");
		_slightPassTex3ID = glGetUniformLocation(_slightPassProgramID, "fDepth");
		_slightPassCameraPosID = glGetUniformLocation(_slightPassProgramID, "cameraPos");
		_slightPassLightColorID = glGetUniformLocation(_slightPassProgramID, "lightColor");
		_slightPassLightPosID = glGetUniformLocation(_slightPassProgramID, "lightPos");
		_slightPassLightDirID = glGetUniformLocation(_slightPassProgramID, "lightFacing");
		_slightPassLightIntensityID = glGetUniformLocation(_slightPassProgramID, "lightIntensity");
		_slightPassLightRangeID = glGetUniformLocation(_slightPassProgramID, "lightRange");
		_slightPassLightAngleID = glGetUniformLocation(_slightPassProgramID, "lightAngle");
	}

	/// <summary>
	/// Helper method for final cleanup
	/// Deletes shaders
	/// </summary>
	void cleanupFramebufferDraw()
	{
		// delete VBOs/VAOs
		glDeleteBuffers(1, &_framebufferDrawVertexBufferID);
		glDeleteVertexArrays(1, &_framebufferDrawVertexArrayID);

		//delete programs
		glDeleteProgram(_framebufferDrawProgramID);
		glDeleteProgram(_plightPassProgramID);
		glDeleteProgram(_slightPassProgramID);
	}

	/// <summary>
	/// Helper method initial setup
	/// Sets up geometry, buffers, and shaders for shadow mapping
	/// </summary>
	void setupShadowMapping()
	{
		/* Set up directional/shadow shader. */
		_shadowPassProgramID = Shaders::LoadShadersShadows();
		//_shaderModelMatrixID = glGetUniformLocation(_programID, "iModelMatrix");
		_shaderMVPMatrixID = glGetUniformLocation(_programID, "iModelViewProjectionMatrix");

		/* Generate FBO for shadow depth buffer. */
		glGenFramebuffers(1, &_shadowFramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _shadowFramebufferID);

		/* Generate the shadow depth buffer. */
		glGenTextures(1, &_shadowFramebufferDepthID);
		glBindTexture(GL_TEXTURE_2D, _shadowFramebufferDepthID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, GlobalPrefs::rShadowMapSize, GlobalPrefs::rShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); //TODO CONST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _shadowFramebufferDepthID, 0);

		glDrawBuffer(GL_NONE);

		/* Unbind the FBO. */
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/// <summary>
	/// Helper method for final cleanup
	/// Deletes shadow mapping buffers, programs, and geometry
	/// </summary>
	void cleanupShadowMapping()
	{
		glDeleteTextures(1, &_shadowFramebufferDepthID);
		glDeleteFramebuffers(1, &_shadowFramebufferID);
		glDeleteProgram(_shadowPassProgramID);
	}

	/// <summary>
	/// Helper method initial setup
	/// Sets up program and buffers for postprocessing
	/// </summary>
	void setupPostProcessing()
	{
		//load shader
		_postProgramID = Shaders::LoadShadersPostProcessing();
		_postProgramTexID = glGetUniformLocation(_postProgramID, "fBuffer");
		_postProgramSmearTexID = glGetUniformLocation(_postProgramID, "sBuffer");
		_postProgramDepthTexID = glGetUniformLocation(_postProgramID, "dBuffer");
		_postProgramBlurAmountID = glGetUniformLocation(_postProgramID, "blurAmount");
		_postProgramDofAmountID = glGetUniformLocation(_postProgramID, "dofAmount");
		_postProgramDofFactorID = glGetUniformLocation(_postProgramID, "dofFactor");
		_postProgramFogAmountID = glGetUniformLocation(_postProgramID, "fogAmount");
		_postProgramFogFactorID = glGetUniformLocation(_postProgramID, "fogFactor");
		_postProgramFogColorID = glGetUniformLocation(_postProgramID, "fogColor");
		
		//load smearbuffer copy shader
		_postCopyProgramID = Shaders::LoadShadersSBCopy();
		_postCopyProgramFactorID = glGetUniformLocation(_postCopyProgramID, "factor");
		_postCopyProgramBlurAmountID = glGetUniformLocation(_postCopyProgramID, "blurAmount");
		_postCopyProgramLastTexID = glGetUniformLocation(_postCopyProgramID, "lBuffer");
		_postCopyProgramSmearTexID = glGetUniformLocation(_postCopyProgramID, "sBuffer");

		//generate base framebuffer FBO and texture
		glGenFramebuffers(1, &_postFramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _postFramebufferID);
		glGenTextures(1, &_postFramebufferTexID);
		glBindTexture(GL_TEXTURE_2D, _postFramebufferTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _renderWidth, _renderHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _postFramebufferTexID, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//generate smearbuffer FBO and texture
		glGenFramebuffers(1, &_postSmearbufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _postSmearbufferID);
		glGenTextures(1, &_postSmearbufferTexID);
		glBindTexture(GL_TEXTURE_2D, _postSmearbufferTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _renderWidth, _renderHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _postSmearbufferTexID, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}

	/// <summary>
	/// Helper method for final cleanup
	/// Deletes postprocessing buffers and program
	/// </summary>
	void cleanupPostProcessing()
	{
		glDeleteTextures(1, &_postFramebufferTexID);
		glDeleteFramebuffers(1, &_postFramebufferTexID);
		glDeleteTextures(1, &_postSmearbufferID);
		glDeleteFramebuffers(1, &_postSmearbufferTexID);

		glDeleteProgram(_postProgramID);
	}

	/// <summary>
	/// Checks the queue and grabs new state information
	/// </summary>
	void checkQueue()
	{
		//lock mutex...
		_mqMutex_p->lock();

		//needs to be sensitive to current state and prioritize certain messages
		//REMEMBER: the "head" of the queue is at 0 and the "tail" at the other end

		if (!_mq_p->empty()) //optimization: skip EVERYTHING if empty
		{
			if (_state == RendererState::idle)
			{
				auto iter = _mq_p->begin();

				//if nothing is loaded, wait for a load call and delete everything else
				for (; iter != _mq_p->end(); iter++)
				{
					std::shared_ptr<Message> msg_sp = *iter;
					if (msg_sp.get()->getType() == MESSAGE_TYPE::RenderLoadMessageType)
					{
						//great, we can load, so load and break!
						startLoad(&static_cast<RenderLoadMessageContent*>(msg_sp.get()->getContent())->data);
						iter = _mq_p->erase(iter); //don't forget to delete that!
						break;
					}
					else
					{
						//remove and log warning
						SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: found a message before load");
						//potential for leak here if it's a render message, but not the culprit in this case
						iter = _mq_p->erase(iter);
					}
				}
			}
			else if (_state == RendererState::loading)
			{
				//we won't do anything until the load is done- not even abort!
			}
			else if (_state == RendererState::rendering)
			{
				//if rendering:
				//	if we have an unload message, ignore other messages, start unload and purge everything before unload
				//	if we have a load message, push to "immediate load" queue and continue rendering as normal
				//  if we don't have either, simply do a render (grab latest render scene and render overlay messages)

				//algorithm:
				//traverse from start to finish:
				//store the latest render message and index
				//store the latest renderOverlay message and index
				//if we hit an unload instruction, store that and break
				//if we hit a loadsingle instruction, queue it and continue
				//if we hit a load instruction, log warning, start unload, store index, and break
				RenderableScene *latestScene = nullptr;
				int latestSceneIndex = -1;
				RenderableOverlay *latestOverlay = nullptr;
				int latestOverlayIndex = -1;
				bool forceAbort = false;
				int abortIndex = -1;

				for (int i = 0; i < _mq_p->size(); i++)
				{
					std::shared_ptr<Message> msg_sp = _mq_p->at(i);
					MESSAGE_TYPE t = msg_sp->getType();

					//DO NOT CHANGE THIS TO A SWITCH
					if (t == MESSAGE_TYPE::RenderDrawMessageType)
					{
						RenderableScene *scn = static_cast<RenderDrawMessageContent*>(msg_sp.get()->getContent())->scene_p;
						if (latestScene != nullptr)
							delete latestScene;
						latestScene = scn;
						latestSceneIndex = i;
						
					}
					else if (t == MESSAGE_TYPE::RenderDrawOverlayMessageType)
					{
						RenderableOverlay *ovl = static_cast<RenderDrawOverlayMessageContent*>(msg_sp.get()->getContent())->overlay_p;
						if (latestOverlay != nullptr)
							delete latestOverlay;
						latestOverlay = ovl;
						latestOverlayIndex = i;
					}
					else if (t == MESSAGE_TYPE::RenderUnloadMessageType)
					{
						abortIndex = i;
						forceAbort = true;
						break;
					}
					else if (t == MESSAGE_TYPE::RenderLoadSingleMessageType)
					{
						RenderLoadSingleMessageContent smc = *static_cast<RenderLoadSingleMessageContent*>(msg_sp.get()->getContent());
						if (!smc.model.name.empty())
						{
							ModelLoadingData mld;
							mld.name = smc.model.name;
							_modelLoadQueue_p->push_back(mld);
						}
						if (!smc.texture.name.empty())
						{
							TextureLoadingData tld;
							tld.name = smc.texture.name;
							_textureLoadQueue_p->push_back(tld);
						}
					}
					else if (t == MESSAGE_TYPE::RenderLoadMessageType)
					{
						SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: Received load message while scene is loaded!");
						abortIndex = i - 1;
						forceAbort = true;
						break;
					}
					else
					{
						SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: Bad message type received!");
					}
				}

				if (forceAbort)
				{
					if (abortIndex >= 0)
					{
						//if abortIndex is a thing, purge everything up and including abortIndex
						for (int i = 0; i < abortIndex; i++)
						{
							std::shared_ptr<Message> msg_sp = _mq_p->at(i);
							MESSAGE_TYPE t = msg_sp->getType();
							if (t == MESSAGE_TYPE::RenderDrawMessageType)
							{
								RenderableScene *scn = static_cast<RenderDrawMessageContent*>(msg_sp.get()->getContent())->scene_p;
								if (scn != nullptr)
									delete scn;
							}
							else if (t == MESSAGE_TYPE::RenderDrawOverlayMessageType)
							{
								RenderableOverlay *ovl = static_cast<RenderDrawOverlayMessageContent*>(msg_sp.get()->getContent())->overlay_p;
								if (ovl != nullptr)
									delete ovl;
							}
						}
						_mq_p->erase(_mq_p->begin(), _mq_p->begin() + abortIndex + 1);						
					}

					startUnload();
				}
				else
				{
					//if renderOverlay comes after render, delete it first, otherwise it'll get deleted normally
					//clear everything up to and including stored index
					if (latestOverlayIndex > latestSceneIndex)
					{
						_mq_p->erase(_mq_p->begin() + latestOverlayIndex);
					}
					_mq_p->erase(_mq_p->begin(), _mq_p->begin() + latestSceneIndex + 1);

					//assign renderablescene and renderableoverlay if they exist
					if (latestScene != nullptr)
					{
						if (_lastScene_p != nullptr)
							delete(_lastScene_p);
						_lastScene_p = latestScene;
					}
					if (latestOverlay != nullptr)
					{
						if (_lastOverlay_p != nullptr)
							delete(_lastOverlay_p);
						_lastOverlay_p = latestOverlay;
					}
				}

			}
			else if (_state == RendererState::unloading)
			{
				//like loading, we won't do anything if unloading
			}
			else
			{
				//well, that shouldn't happen
				SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: Ended up in an impossible state");
			}
			
		}
		//if currently loading or unloading, ignore messages

		//unlock mutexs
		_mqMutex_p->unlock();
	}


	void startLoad(RenderableSetupData *data)
	{
		SDL_Log("Renderer: Entering load state");

		_state = RendererState::loading;

		//just start the load here
		for (auto mdata : data->models)
		{
			if (!mdata.empty())
			{
				ModelLoadingData mld;
				mld.name = mdata;
				_modelLoadQueue_p->push_back(mld);
			}
		}

		for (auto tdata : data->textures)
		{
			if (!tdata.empty())
			{
				TextureLoadingData tld;
				tld.name = tdata;
				_textureLoadQueue_p->push_back(tld);
			}
		}

		//grab context
		acquireContext();

		//DON'T DO ANYTHING LONG IN HERE BECAUSE THE QUEUE IS STILL BLOCKED
	}

	void startUnload()
	{
		_state = RendererState::unloading;
	}

	/// <summary>
	/// Executed in load state
	/// Acquires context, dispatches file load messages, and handles incoming file load messages
	/// </summary>
	void doLoad()
	{
		//needs refactoring badly

		//possible future optimization: do things you don't need context for when you don't have context

		//if we don't have context, get context
		if (!haveContext())
			acquireContext();
		if (!haveContext())
			return; //wait for context to be released

		//TODO EVENTUALLY: blit render load screen
		drawLoadScreen();

		//loads stuff

		MessagingSystem *ms = &MessagingSystem::instance();
		if (ms == nullptr)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: messaging system does not exist");
			return;
		}

		//possible future optimization: don't push the entire queues at once
		//dispatch from model and texture load queues
		if (!_modelLoadQueue_p->empty())
		{
			for (int i = 0; i < _modelLoadQueue_p->size(); i++)
			{
				ModelLoadingData mld = _modelLoadQueue_p->at(i);

				FileLoadMessageContent *flmc = new FileLoadMessageContent();
				flmc->path = MODEL_BASEPATH_CONST + mld.name + MODEL_EXTENSION_CONST;
				flmc->relative = true;

				mld.hash = FileEngine::HashFilePath(flmc->path, flmc->relative);

				std::shared_ptr<Message> msg = std::make_shared<Message>(MESSAGE_TYPE::FileLoadMessageType, false);
				msg->setContent(flmc);
				ms->postMessage(msg);

				_modelAwaitQueue_p->push_back(mld);
			}

			_modelLoadQueue_p->clear();
		}

		if (!_textureLoadQueue_p->empty())
		{
			for (int i = 0; i < _textureLoadQueue_p->size(); i++)
			{
				TextureLoadingData tld = _textureLoadQueue_p->at(i);

				FileLoadImageMessageContent *flimc = new FileLoadImageMessageContent();
				flimc->path = TEXTURE_BASEPATH_CONST + tld.name + TEXTURE_EXTENSION_CONST;
				flimc->relative = true;

				tld.hash = FileEngine::HashFilePath(flimc->path, flimc->relative);

				std::shared_ptr<Message> msg = std::make_shared<Message>(MESSAGE_TYPE::FileLoadImageMessageType, false);
				msg->setContent(flimc);
				ms->postMessage(msg);

				_textureAwaitQueue_p->push_back(tld);
			}

			_textureLoadQueue_p->clear();
		}

		//process results from file return queue
		//not optimized well for concurrency
		if (!_fmq_p->empty())
		{
			_fmqMutex_p->lock();

			for (int i = 0; i < _fmq_p->size(); i++)
			{
				Message *msg = _fmq_p->at(i).get();
				BaseMessageContent * bmc = msg->getContent();

				switch (msg->getType())
				{
				case MESSAGE_TYPE::FileLoadedMessageType:
					{
						FileLoadedMessageContent *flmc = static_cast<FileLoadedMessageContent*>(bmc);
						findAndLoadModel(flmc);
					}
					break;
				case MESSAGE_TYPE::FileLoadedImageMessageType:
					{
						FileLoadedImageMessageContent *flimc = static_cast<FileLoadedImageMessageContent*>(bmc);
						findAndLoadTexture(flimc);
					}
					break;
				default:
					SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Renderer: Received unknown message content type ");
					break;
				}
			}

			_fmq_p->clear();

			_fmqMutex_p->unlock();
		}

		//loading is done if and only if both load and await queues are empty and we have context
		if (_textureLoadQueue_p->empty() && _textureAwaitQueue_p->empty() && _modelLoadQueue_p->empty() && _modelAwaitQueue_p->empty() && haveContext())
		{
			std::shared_ptr<Message> msg = std::make_shared<Message>(MESSAGE_TYPE::RenderReadyMessageType, false);
			msg->setContent(new RenderReadyMessageContent());
			ms->postMessage(msg);
			
			SDL_Log("Renderer: Entering render state");
			_state = RendererState::rendering;
		}

	}

	/// <summary>
	/// Executed in render state
	/// Handles a single load message and checks the file queue once
	/// Not yet implemented
	/// </summary>
	void doSingleLoad()
	{
		//load one thing during drawing process
		//TODO implementation

		//dispatch ONE model and ONE texture from load queues (if nonempty)

		//check the file return queue for ONE file
	}

	/// <summary>
	/// Helper method for model loading
	/// Loads a model from file loaded message, finding the matching model await data and binding it
	/// </summary>
	void findAndLoadModel(FileLoadedMessageContent *flmc)
	{
		int64_t foundModel = -1;
		ModelLoadingData foundMLD;

		for (int j = 0; j < _modelAwaitQueue_p->size(); j++)
		{
			ModelLoadingData mld = _modelAwaitQueue_p->at(j);
			if (mld.hash == flmc->hash)
			{
				foundMLD = mld;
				foundModel = j;
				break;
			}
		}

		if (foundModel >= 0)
		{
			loadOneModel(foundMLD, &flmc->content);
			_modelAwaitQueue_p->erase(_modelAwaitQueue_p->begin() + foundModel);
		}
	}

	/// <summary>
	/// Helper method for texture loading
	/// Loads a texture from file loaded message, finding the matching texture await data and binding it
	/// </summary>
	void findAndLoadTexture(FileLoadedImageMessageContent *flimc)
	{
		int64_t foundTexture = -1;
		TextureLoadingData foundTLD;

		for (int j = 0; j < _textureAwaitQueue_p->size(); j++)
		{
			TextureLoadingData tld = _textureAwaitQueue_p->at(j);
			if (tld.hash == flimc->hash)
			{
				foundTLD = tld;
				foundTexture = j;
				break;
			}
		}

		if (foundTexture >= 0)
		{
			loadOneTexture(foundTLD, flimc->image.get());
			_textureAwaitQueue_p->erase(_textureAwaitQueue_p->begin() + foundTexture);
		}
	}

	/// <summary>
	/// Helper method for model loading
	/// Converts and binds a single model, then pushes to model list
	/// </summary>
	void loadOneModel(ModelLoadingData mld, std::string *data_p)
	{

		ModelData md;

		//does nothing yet
		auto objData = OBJImport::importObjInfo(*data_p);
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

		_models_p->emplace(mld.name, md);

	}

	/// <summary>
	/// Helper method for texture loading
	/// Converts and binds a single texture, then pushes to texture list
	/// </summary>
	void loadOneTexture(TextureLoadingData tld, SDL_Surface *image_p)
	{
		TextureData td;

		GLint mode = GL_RGB;

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

		_textures_p->emplace(tld.name, td);
		
	}

	/// <summary>
	/// Executed in unload state
	/// Unloads everything and returns to idle state
	/// </summary>
	void doUnload()
	{
		//TODO EVENTUALLY: blit render load screen
		drawUnloadScreen();

		//delete (some) GL stuff, purge data structures, DO NOT PURGE MESSAGE QUEUES
		unloadGL();
		unloadData();

		//finally release context
		releaseContext();

		_state = RendererState::idle;
	}

	/// <summary>
	/// Executed in idle state
	/// Does nothing
	/// </summary>
	void doIdle()
	{
		//drawIdleScreen();
	}

	/// <summary>
	/// Helper method for scene unload
	/// Unbinds all VBOs, VAOs, and texture objects from OpenGL
	/// </summary>
	void unloadGL()
	{
		//unbind all OGL

		//delete VBOs and VAOs
		for (std::map<std::string, ModelData>::iterator itr = _models_p->begin(); itr != _models_p->end(); itr++)
		{
			ModelData md = itr->second;
			glDeleteVertexArrays(1, &md.vaoID);
			glDeleteBuffers(1, &md.vboID);
		}

		//delete textures
		for (std::map<std::string, TextureData>::iterator itr = _textures_p->begin(); itr != _textures_p->end(); itr++)
		{
			TextureData td = itr->second;
			glDeleteTextures(1, &td.texID);
		}
	}

	/// <summary>
	/// Helper method for scene unload
	/// Clears input and file message queues, clears load and await queues, clears resource lists and purges scene and overlay data
	/// </summary>
	void unloadData()
	{
		//clear (but DO NOT DELETE) data structures

		//purge input and file, but NOT main message queue
		_imqMutex_p->lock();
		_imq_p->clear();
		_imqMutex_p->unlock();
		_fmqMutex_p->lock();
		_fmq_p->clear();
		_fmqMutex_p->unlock();

		//purge load queues
		_modelLoadQueue_p->clear();
		_textureLoadQueue_p->clear();

		//purge await queues
		_modelAwaitQueue_p->clear();
		_textureAwaitQueue_p->clear();

		//purge model and texture lists
		_models_p->clear();
		_textures_p->clear();

		//purge current scene and overlay
		if(_lastScene_p != nullptr)
			delete(_lastScene_p);
		_lastScene_p = nullptr;

		if(_lastOverlay_p != nullptr)
			delete(_lastOverlay_p);
		_lastOverlay_p = nullptr;
	}

	/// <summary>
	/// Helper method for load screen
	/// Draws a simple green screen
	/// </summary>
	void drawLoadScreen()
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
	void drawUnloadScreen()
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
	void drawIdleScreen()
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
	void doRender()
	{

		//will remain in final
		if (_lastScene_p == nullptr)
		{
			drawNullScene();
		}
		else
		{
			drawCamera(_lastScene_p); //set up the camera
			drawObjects(_lastScene_p); //do the geometry pass
			drawShadows(_lastScene_p); //do the shadow map
			drawLighting(_lastScene_p); //do the lighting pass
			drawPostProcessing(_lastScene_p); //do the postprocessing
		}

		if (_lastOverlay_p == nullptr)
		{
			drawNullOverlay();
		}
		else
		{
			drawOverlay(_lastOverlay_p);
		}

		//TODO vsync/no vsync
		//SDL_GL_SwapWindow(_window_p); //will be moved back here once idle screen problems can be fixed
	}

	/// <summary>
	/// Draws a blank scene
	/// Used when no scene data is available
	/// </summary>
	void drawNullScene()
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
	void drawCamera(RenderableScene *scene)
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

	}

	/// <summary>
	/// Draws all objects in the scene
	/// Does some setup, then calls drawObject for each object
	/// Essentially the geometry pass of a deferred renderer
	/// </summary>
	void drawObjects(RenderableScene *scene)
	{

		//bind framebuffer and clear
		glBindFramebuffer(GL_FRAMEBUFFER, _framebufferID);
		glViewport(0, 0, _renderWidth, _renderHeight);

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
	void drawObject(RenderableObject *object)
	{
		//draw one arbitraty object
		//NOTE: should always be tolerant of missing resources!

		//set shader program
		glUseProgram(_programID);

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
		
		if(!hasModel)
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
				glUniform1i(_shaderTextureID, 0);
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
			glUniform1i(_shaderTextureID, 0);
		}

		//try to bind normal map
		if (hasNTexture)
		{
			texData = _textures_p->find(object->normalName)->second;
			if (texData.texID != 0)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texData.texID);
				glUniform1i(_shaderNormalID, 1);
				glUniform1i(_shaderHasNormID, GL_TRUE);
			}
			else
			{
				hasNTexture = false;
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Renderer: texture has no texID!");
			}
		}

		if (!hasNTexture)
		{
			glUniform1i(_shaderHasNormID, GL_FALSE);
		}

		glUniform1f(_shaderSmoothnessID, object->smoothness);

		//transform!
		glm::mat4 objectMVM = glm::mat4();
		objectMVM = glm::translate(objectMVM, object->position);
		//SDL_Log("%f, %f, %f", object->position.x, object->position.y, object->position.z);
		objectMVM = glm::scale(objectMVM, object->scale);
		objectMVM = glm::rotate(objectMVM, object->rotation.y, glm::vec3(0, 1, 0));
		objectMVM = glm::rotate(objectMVM, object->rotation.x, glm::vec3(1, 0, 0));
		objectMVM = glm::rotate(objectMVM, object->rotation.z, glm::vec3(0, 0, 1));
		glm::mat4 objectMVPM = _baseModelViewProjectionMatrix *  objectMVM;
		glm::mat4 objectMVM2 = _baseModelViewMatrix * objectMVM;
		glUniformMatrix4fv(_shaderMVPMatrixID, 1, GL_FALSE, &objectMVPM[0][0]);
		glUniformMatrix4fv(_shaderModelMatrixID, 1, GL_FALSE, &objectMVM[0][0]);

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
	void drawShadows(RenderableScene *scene)
	{

		//grab the main light
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
		glUseProgram(_shadowPassProgramID);
		glBindFramebuffer(GL_FRAMEBUFFER, _shadowFramebufferID);
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
			glm::mat4 objectMVPM = _depthModelViewProjectionMatrix *  objectMVM;
			glUniformMatrix4fv(_shadowPassMVPMatrixID, 1, GL_FALSE, &objectMVPM[0][0]);
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
	void drawLighting(RenderableScene *scene)
	{
		
		//setup framebuffer
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, _postFramebufferID);

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
	void drawLightingMainPass(RenderableScene *scene) //will need more args
	{
		//bind shader
		glUseProgram(_framebufferDrawProgramID);

		//bind buffers
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture0ID);
		//glBindTexture(GL_TEXTURE_2D, _shadowFramebufferDepthID);
		glUniform1i(_framebufferDrawTex0ID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture1ID);
		glUniform1i(_framebufferDrawTex1ID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture2ID);
		glUniform1i(_framebufferDrawTex2ID, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _framebufferDepthID);
		glUniform1i(_framebufferDrawTex3ID, 3);

		//bind shadow mapping buffer
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, _shadowFramebufferDepthID); 
		glUniform1i(_framebufferDrawTexSID, 4);

		//calculate and bind shadow bias matrix
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
		glm::mat4 depthBiasMVP = biasMatrix * _depthModelViewProjectionMatrix;  
		//glm::mat4 depthBiasMVP = glm::inverse(_depthModelViewProjectionMatrix);
		glUniformMatrix4fv(_framebufferDrawBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

		//bind clear color		
		glm::vec3 clear = scene->camera.clearColor;
		glUniform3f(_framebufferDrawColorID, clear.r, clear.g, clear.b);

		//bind ambient light
		glm::vec3 ambient = computeAmbientLight(scene);
		glUniform3f(_framebufferDrawAmbientID, ambient.r, ambient.g, ambient.b);

		//bind directional light color
		glm::vec3 directional = _mainDirectionalLight.color * _mainDirectionalLight.intensity;
		glUniform3f(_framebufferDrawDirColorID, directional.r, directional.g, directional.b);

		//bind directional light facing
		glm::mat4 rotMatrix = glm::mat4();
		rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.y, glm::vec3(0, 1, 0));
		rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.x, glm::vec3(1, 0, 0));
		rotMatrix = glm::rotate(rotMatrix, _mainDirectionalLight.rotation.z, glm::vec3(0, 0, 1));
		glm::vec3 lightFacing = rotMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		glUniform3f(_framebufferDrawDirFacingID, lightFacing.x, lightFacing.y, lightFacing.z); 

		//bind camera position
		glm::vec3 cPos = scene->camera.position;
		glUniform3f(_framebufferDrawCameraPosID, cPos.x, cPos.y, cPos.z);

		//setup vertices
		glBindVertexArray(_framebufferDrawVertexArrayID);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//unbind
		glBindVertexArray(0);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	/// <summary>
	/// Draws a single point light in a lighting pass (using fullscreen quad for now)
	/// </summary>
	void drawLightingPointLight(RenderableLight light, RenderableScene *scene)
	{
		glUseProgram(_plightPassProgramID);

		//bind buffers
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture0ID);
		glUniform1i(_plightPassTex0ID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture1ID);
		glUniform1i(_plightPassTex1ID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture2ID);
		glUniform1i(_plightPassTex2ID, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _framebufferDepthID);
		glUniform1i(_plightPassTex3ID, 3);

		//bind camera and lighting data
		glm::vec3 cPos = scene->camera.position;
		glUniform3f(_plightPassCameraPosID, cPos.x, cPos.y, cPos.z);
		glm::vec3 lPos = light.position;
		glUniform3f(_plightPassLightPosID, lPos.x, lPos.y, lPos.z);
		glm::vec3 lColor = light.color;
		glUniform3f(_plightPassLightColorID, lColor.x, lColor.y, lColor.z);
		glUniform1f(_plightPassLightIntensityID, light.intensity);
		glUniform1f(_plightPassLightRangeID, light.range);

		//setup vertices
		glBindVertexArray(_framebufferDrawVertexArrayID);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//unbind
		glBindVertexArray(0);
	}

	/// <summary>
	/// Draws a single spot light in a lighting pass
	/// </summary>
	void drawLightingSpotLight(RenderableLight light, RenderableScene *scene)
	{
		//prepare direction vector for "rotation"
		glm::mat4 rotMatrix = glm::mat4();
		rotMatrix = glm::rotate(rotMatrix, light.rotation.y, glm::vec3(0, 1, 0));
		rotMatrix = glm::rotate(rotMatrix, light.rotation.x, glm::vec3(1, 0, 0));
		rotMatrix = glm::rotate(rotMatrix, light.rotation.z, glm::vec3(0, 0, 1));
		glm::vec3 lightFacing = rotMatrix * glm::vec4(0.0f, 0.0f, 1.0f , 1.0f);

		glUseProgram(_slightPassProgramID);

		//bind buffers
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture0ID);
		glUniform1i(_slightPassTex0ID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture1ID);
		glUniform1i(_slightPassTex1ID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _framebufferTexture2ID);
		glUniform1i(_slightPassTex2ID, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, _framebufferDepthID);
		glUniform1i(_slightPassTex3ID, 3);

		//bind camera and lighting data
		glm::vec3 cPos = scene->camera.position;
		glUniform3f(_slightPassCameraPosID, cPos.x, cPos.y, cPos.z);

		glm::vec3 lPos = light.position;
		glUniform3f(_slightPassLightPosID, lPos.x, lPos.y, lPos.z);

		glUniform3f(_slightPassLightDirID, lightFacing.x, lightFacing.y, lightFacing.z);

		glm::vec3 lColor = light.color;
		glUniform3f(_slightPassLightColorID, lColor.x, lColor.y, lColor.z);
		glUniform1f(_slightPassLightIntensityID, light.intensity);
		glUniform1f(_slightPassLightRangeID, light.range);
		glUniform1f(_slightPassLightAngleID, light.angle);

		//setup vertices
		glBindVertexArray(_framebufferDrawVertexArrayID);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//unbind
		glBindVertexArray(0);

	}

	/// <summary>
	/// Helper method for scene lighting
	/// Computes ambient light values
	/// </summary>
	glm::vec3 computeAmbientLight(RenderableScene *scene)
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
	/// Applies postprocessing and blits buffers
	/// May be broken up with helper methods later
	/// </summary>
	void drawPostProcessing(RenderableScene *scene)
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

		glUseProgram(_postProgramID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _postFramebufferTexID);
		glUniform1i(_postProgramTexID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _postSmearbufferTexID);
		glUniform1i(_postProgramSmearTexID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _framebufferDepthID);
		glUniform1i(_postProgramDepthTexID, 2);
		   
		glUniform1f(_postProgramBlurAmountID, blurAmount);
		glUniform1f(_postProgramDofAmountID, dofAmount);
		glUniform1f(_postProgramDofFactorID, dofFactor);
		glUniform1f(_postProgramFogAmountID, fogAmount);
		glUniform1f(_postProgramFogFactorID, fogFactor);
		glUniform3f(_postProgramFogColorID, fogColor.r, fogColor.g, fogColor.b); 
		 
		glBindVertexArray(_framebufferDrawVertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		drawPostProcessingCopySmearbuffer(blurFactor, blurAmount); 

	}

	void drawPostProcessingCopySmearbuffer(float blurFactor, float blurAmount)
	{
		//copy smearbuffer into (temporary) texture
		GLuint sBufferTexture = 0;
		glGenTextures(1, &sBufferTexture);
		glBindTexture(GL_TEXTURE_2D, sBufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _renderWidth, _renderHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _postSmearbufferID);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 0, 0, _renderWidth, _renderHeight, 0);

		//blend into smearbuffer with shader
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, _postSmearbufferID);
		//glBlitFramebuffer(0, 0, _renderWidth, _renderHeight, 0, 0, _renderWidth, _renderHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glViewport(0, 0, _renderWidth, _renderHeight); 

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_postCopyProgramID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _postFramebufferTexID);
		glUniform1i(_postCopyProgramLastTexID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, sBufferTexture);
		glUniform1i(_postCopyProgramSmearTexID, 1);

		glUniform1f(_postCopyProgramFactorID, blurFactor);
		glUniform1f(_postCopyProgramBlurAmountID, blurAmount);
		
		glBindVertexArray(_framebufferDrawVertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//delete temporary texture
		glDeleteTextures(1, &sBufferTexture);
	}

	/// <summary>
	/// Draws the overlay
	/// TODO needs implementation
	/// </summary>
	void drawOverlay(RenderableOverlay *overlay)
	{
		//TODO draw overlay

		//need to make sure right framebuffer is set, clear depth but not color
	}

	/// <summary>
	/// Draws the null overlay
	/// Run if no overlay data is available, literally does nothing
	/// </summary>
	void drawNullOverlay()
	{
		//probably just leave it blank yeah
	}

	/// <summary>
	/// Helper method for context
	/// Attempts to acquire OpenGL context, returns true on success
	/// </summary>
	bool acquireContext()
	{
		if (SDL_GL_GetCurrentContext() == _context_p)
		{
			return true; //we already have it!
		}
		else
		{
			int responseCode = SDL_GL_MakeCurrent(_window_p, _context_p);
			if (responseCode == 0)
				return true;
		}

		return false;
	}

	/// <summary>
	/// Helper method for context
	/// Releases OpenGL context so others can use it
	/// </summary>
	bool releaseContext()
	{
		//does nothing but we may need it later
		return true;
	}

	/// <summary>
	/// Helper method for context
	/// Checks if the renderer has OpenGL context
	/// </summary>
	bool haveContext()
	{
		return SDL_GL_GetCurrentContext() == _context_p;
	}

};

//Base class implementations below: just passes through because PIMPL

RenderEngine::RenderEngine()
{
	_impl = new RenderEngineImplementation();
}

void RenderEngine::start()
{
	_impl->start();
}

void RenderEngine::update()
{
	_impl->update();
}

RenderEngine::~RenderEngine()
{
	delete(_impl);
};
