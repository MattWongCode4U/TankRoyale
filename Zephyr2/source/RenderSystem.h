#pragma once
#include "System.h"
#include "GameObject.h"
#include "renderer\RenderPipeline.h"
#include <mutex>
#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>
#include <SDL_opengl.h>
#include <freeglut.h>
#include <math.h>  

#include "System.h"
#include "GameObject.h"


class RenderSystem : public System {
public:
	RenderSystem(MessageBus* mbus);
	~RenderSystem();

	struct renderObj {
		string ID, sprite;
		float x, y, z, orientation, w, h;
	};

	void handleMessage(Msg * msg);
	void startSystemLoop();
	void stopSystemLoop();
	void init();
	SDL_Window* GetSDLWindow();

	int loadedLevel = 0;

	//Window constants
	GLint XSTART = 100, YSTART = 100, WIDTH = 1000, HEIGHT = 800; //Constants for drawing the window
	GLfloat GAMEWIDTH = 200.0f, GAMEHEIGHT = 200.0f, //Amount of sprites that can fit in X and Y respectively
				MAX_X = 200.0f, MAX_Y = 200.0f; //Amount of "x" and "y" coordinates in our world (x2 for - values)
	GLfloat aspectRatio;
	const int timeFrame = 33;

	//Camera constants
	float CAMERAPAN_X = 30.0f, CAMERAPAN_Y = 30.0f;
	float minCameraX = -330.0f, maxCameraX = 330.0f, cameraX = 0.0f, 
		minCameraY = -330.0f, maxCameraY = 330.0f, cameraY = 0.0f;
private:
	bool running;
	RendererState state;
	
	RenderPipeline *pipeline;
	SDL_Window *window;
	std::mutex mtx;
	
	std::map<string, RenderableObject> *objects;
	RenderableScene *scene;
	RenderableOverlay *overlay;

	void addObjectToRenderList(Msg *m);
	void removeObjectFromRenderList(Msg *m);
	void updateObjPosition(Msg *m);
	void updateObjSprite(Msg *m);
	void updateObjRender(Msg *m);
	void updateHealthHUD(Msg *m);
	void updatePipelineConfig(Msg *m);

	void panLeft();
	void panRight();
	void panUp();
	void panDown();
	void cameraToPlayer();
	void positionUpdated();

	void levelLoaded(Msg*m);
	std::pair<std::string,RenderableObject> parseObject(std::string data);
};