#include "RenderSystem.h"
#include "renderer\RenderPipeline.h"

/*
	RenderSystem Constructor.
*/
RenderSystem::RenderSystem(MessageBus* mbus) : System(mbus) {
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING); //TODO move to main

	aspectRatio = (GLfloat)(WIDTH) / (GLfloat)(HEIGHT);
	window = SDL_CreateWindow("Zephyr2", RenderSystem::XSTART, RenderSystem::YSTART, RenderSystem::WIDTH, RenderSystem::HEIGHT, SDL_WINDOW_OPENGL);

	SDL_GL_SwapWindow(window); //needed?

	//create pipeline, scene, overlay
	pipeline = new RenderPipeline(window);
	scene = new RenderableScene();
	overlay = new RenderableOverlay();
	objects = new map<string, RenderableObject>();
}

/*
	RenderSystem destructor.
*/
RenderSystem::~RenderSystem() {
	delete pipeline;
	delete scene;
	delete objects;
	SDL_DestroyWindow(window);
}

/*
	Initialize RenderSystem.
*/
void RenderSystem::init() {

	//TEMPORARY: setup initial camera rig and main light	
		

}

/*
	Start of the system.
*/
void RenderSystem::startSystemLoop() {

	//initialize self and pipeline
	init();
	pipeline->setupGLOnThread();
	pipeline->setupSceneOnThread();
	
	//TODO TEMPORARY loadall resources
	state = RendererState::loading;
	pipeline->loadAllResources();

	//TEMPORARY: set state
	state = RendererState::rendering;

	while (running) { //TODO change to "alive"?
		

		handleMsgQ();

		//TODO eliminate this copy operation
		scene->objects.clear();
		for (auto el : *objects)
		{
			scene->objects.push_back(el.second);
		}

		pipeline->doRender(scene, overlay);
		
		SDL_GL_SwapWindow(window);
	}

	//TODO cleaner shutdown?
}

/*
	Shutdown of system.
*/
void RenderSystem::stopSystemLoop() {
	state = RendererState::unloading;
	running = false;

	pipeline->cleanupSceneOnThread();
	pipeline->cleanupGLOnThread();
	

	SDL_DestroyWindow(window);
	SDL_Quit(); //you realize this ENDS THE PROGRAM, right?

}

/*
	RenderSystem handle messages from the message bus.
*/
void RenderSystem::handleMessage(Msg *msg) {
	std::string s = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));

	////Display Thread ID for Debugging
	//OutputDebugString("Render  Handle Messsage on thread: ");
	//OutputDebugString(s.c_str());
	//OutputDebugString("\n");
	// call the parent first 
	System::handleMessage(msg);

	// personal call 
	switch (msg->type) {
	case UPDATE_OBJECT_POSITION:
		//mtx.lock();
		updateObjPosition(msg);
	//	mtx.unlock();
		break;
	case GO_ADDED:
		// parse data from msg to add obj to list
		//mtx.lock();
		addObjectToRenderList(msg);
		//mtx.unlock();
		break;
	case GO_REMOVED:
	//	mtx.lock();
		removeObjectFromRenderList(msg);
	//	mtx.unlock();
		break;
	case UPDATE_OBJ_SPRITE:
		updateObjSprite(msg);
		break;
	case UPDATE_HP_BAR:
		updateHealthHUD(msg);
		break;
	case LEVEL_LOADED:
		levelLoaded(msg);
		break;
	//PANNING CAMERA

	/*case SPACEBAR_PRESSED:
		if (loadedLevel == 2) {
			cameraToPlayer();
		}
		break;
	case UP_ARROW_PRESSED:
		if (loadedLevel == 2) {
			panUp();
			positionUpdated();
		}
		break;
	case DOWN_ARROW_PRESSED: 
		if (loadedLevel == 2) {
			panDown();
			positionUpdated();
		}
		break;
	case RIGHT_ARROW_PRESSED:
		if (loadedLevel == 2) {
			panRight();
			positionUpdated();
		}
		break;
	case LEFT_ARROW_PRESSED:
		if (loadedLevel == 2) {
			panLeft();
			positionUpdated();
		}
		break;*/
	default:
		break;
	}
}

/*
	Parse message and create a RenderableObject
*/
RenderableObject RenderSystem::parseObject(std::string data)
{
	RenderableObject obj;

	//derived from old renderObject implementation, could be optimized

	return obj;
}

/*
	Send message that the camera was panned.
*/
void RenderSystem::positionUpdated() {
	Msg* m = new Msg(CAMERA_OFFSET, "");
	std::ostringstream oss;
	oss << to_string(cameraX) << "," << to_string(cameraX);
	m->data = oss.str();
	msgBus->postMessage(m, this);
}

/*
	Remove object from the list of objects to render.
*/
void RenderSystem::removeObjectFromRenderList(Msg* m) {
	
}

/*
	Add object to the list of objects to render.
*/
void RenderSystem::addObjectToRenderList(Msg* m) {
	
}

/*
	Update the position of an object given in the message.
*/
void RenderSystem::updateObjPosition(Msg* m) {
	
}

/*
	Update the sprite of an object
*/
void RenderSystem::updateObjSprite(Msg* m) {
	
}

/*
	Update the health bar on the HUD.
*/
void RenderSystem::updateHealthHUD(Msg* m) {
	
}

/*
	Pan the camera left.
*/
void RenderSystem::panLeft() {
	if (cameraX + CAMERAPAN_X <= maxCameraX) {
		cameraX += CAMERAPAN_X;
	}
}

/*
	Pan the camera right.
*/
void RenderSystem::panRight() {
	if (cameraX - CAMERAPAN_X >= minCameraX) {
		cameraX -= CAMERAPAN_X;
	}
}

/*
	Pan the camera up.
*/
void RenderSystem::panUp() {
	if (cameraY - CAMERAPAN_Y >= minCameraY) {
		cameraY -= CAMERAPAN_Y;
	}
}

/*
	Pan the camera down.
*/
void RenderSystem::panDown() {
	if (cameraY + CAMERAPAN_Y <= maxCameraY) {
		cameraY += CAMERAPAN_Y;
	}
}

/*
	Center the camera on the player.
*/
void RenderSystem::cameraToPlayer() {
	//TODO reimplement if necessary
}

/*
	Handle message when the level is loaded.
*/
void RenderSystem::levelLoaded(Msg* m) {
	state = RendererState::rendering;
	loadedLevel = atoi(m->data.c_str());
	if (loadedLevel != 2) { //Reset camera when not in game
		cameraX = 0.0f;
		cameraY = 0.0f;
	}
}

