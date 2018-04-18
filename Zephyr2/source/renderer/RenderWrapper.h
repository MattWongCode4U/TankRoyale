#pragma once
#include <SDL.h>
#include "..\MessageBus.h"
#include "..\RenderSystem.h"
#include "RenderableTypes.h"
#include "RendererInternalTypes.h"
#include "RenderPipeline.h"


const float CAMERA_PAN_STEP = 1.0f;

/*
RenderWrapper sits between the RenderPipeline and the rest of the engine.
It handles renderer state, messaging, and most of the object management
This is largely an adapter between RACE and Zephyr paradigms
Further information in RenderWrapper.cpp
*/
class RenderWrapper {
public:
	RenderWrapper(RenderSystem *system);
	~RenderWrapper();


	void handleMessage(Msg * msg);
	void startSystemLoop();
	void stopSystemLoop();
	void init();
	SDL_Window* GetSDLWindow();

	

private:
	bool running;
	int loadedLevel = 0;
	RendererState state;

	//pointers to managed objects
	RenderSystem *system;
	RenderPipeline *pipeline;
	SDL_Window *window;
	std::mutex mtx;

	//list of objects handled
	std::map<std::string, RenderableObject> *objects;
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
	std::pair<std::string, RenderableObject> parseObject(std::string data);
};