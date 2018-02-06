#include "RenderSystem.h"
#include "renderer\RenderPipeline.h"
#include "renderer\GlobalPrefs.h"

/*
	RenderSystem Constructor.
*/
RenderSystem::RenderSystem(MessageBus* mbus) : System(mbus) {
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING); //TODO move to main

	aspectRatio = (GLfloat)(GlobalPrefs::windowWidth) / (GLfloat)(GlobalPrefs::windowHeight);
	window = SDL_CreateWindow("Zephyr2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GlobalPrefs::windowWidth, GlobalPrefs::windowHeight, SDL_WINDOW_OPENGL);

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
	RenderableCamera cam;
	cam.farPlane = 1000;
	cam.nearPlane = 1;
	cam.clearColor = glm::vec3(0.25f, 0.25f, 0.25f);
	cam.viewAngle = 60.0f;
	cam.position = glm::vec3(0, 0, 100.0f);
	cam.rotation = glm::vec3(0, 0, 0);
	scene->camera = cam;

	RenderableLight mainLight;
	mainLight.type = RenderableLightType::AMBIENT;
	mainLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
	mainLight.intensity = 1.0f;
	scene->lights.push_back(mainLight);
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
	running = true;



	while (running)
	{ //TODO change to "alive"?
		

		handleMsgQ();

		//TODO can we do this without copying?
		//TODO pointers?!
		scene->objects.clear();
		overlay->elements.clear();
		for (auto el : *objects)
		{
			RenderableObject &obj = el.second;

			switch (obj.type)
			{
			case RenderableType::OBJECT3D:
				scene->objects.push_back(obj);
				break;
			case RenderableType::OVERLAY:
				overlay->elements.push_back(obj);
				break;
			}			
			
			
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
	case UPDATE_OBJ_RENDER:
		updateObjRender(msg);
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
	Parse message and create an id/RenderableObject pair
*/
std::pair<std::string, RenderableObject> RenderSystem::parseObject(std::string data)
{
	SDL_Log(data.c_str());

	vector<string> objectData = split(data, ',');

	RenderableObject obj;
	std::string id;

	//derived from old renderObject implementation, could be optimized
	//TODO need to redefine object data message
	std::string sprite, model, normal;
	float x, y, z, orientation, w, h, smoothness;
	int frames = 1;
	int type = (int)RenderableType::OBJECT3D;

	id = objectData[0];
	sprite = objectData[1];
	std::string spriteName = std::string(sprite, 0, sprite.find_last_of('.')); //strip file extension TODO handle no extension
	x = (float)(atof(objectData[2].c_str()));
	y = (float)(atof(objectData[3].c_str()));
	z = (float)(atof(objectData[4].c_str()));
	orientation = (float)(atof(objectData[5].c_str()));
	w = (float)(atof(objectData[6].c_str()));
	h = (float)(atof(objectData[7].c_str())); 
	frames = atoi(objectData[9].c_str());
	type = atoi(objectData[10].c_str());
	model = objectData[11];
	normal = objectData[12];
	smoothness = (float)(atof(objectData[13].c_str()));

	//set obj data
	obj.type = (RenderableType)type;
	obj.position = glm::vec3(x, y, z);
	obj.rotation = glm::vec3(0, 0, glm::radians(orientation));
	obj.scale = glm::vec3(w, h, 1);
	obj.modelName = model;
	obj.albedoName = spriteName;
	obj.normalName = normal;
	obj.smoothness = smoothness;
	obj.frameCount = frames;

	return std::make_pair(id, obj);
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
	std::string keyToRemove = m->data;
	objects->erase(keyToRemove);
}

/*
	Add object to the list of objects to render.
*/
void RenderSystem::addObjectToRenderList(Msg* m) {
	auto objectPair = parseObject(m->data);
	objects->emplace(objectPair);
	//that's more like it!
}

/*
	Update the position of an object given in the message.
*/
void RenderSystem::updateObjPosition(Msg* m) {
	SDL_Log(m->data.c_str());
	std::vector<std::string> dataVector = split(m->data, ',');

	//id,renderable,x,y,z,orientation,width,length,physEnabled,type
	std::string id = dataVector[0];

	RenderableObject *obj = &objects->at(id);
	obj->position = glm::vec3((float)(atof(dataVector[2].c_str())), (float)(atof(dataVector[3].c_str())), (float)(atof(dataVector[4].c_str())));
	obj->rotation = glm::vec3(0, 0, glm::radians((float)(atof(dataVector[5].c_str()))));
	obj->scale = glm::vec3((float)(atof(dataVector[6].c_str())), (float)(atof(dataVector[7].c_str())), 1.0f);
}

/*
	Update the sprite of an object
*/
void RenderSystem::updateObjSprite(Msg* m)
{
	SDL_Log(m->data.c_str());
	std::vector<std::string> dataVector = split(m->data, ',');
	std::string id = dataVector.at(0);
	std::string sprite = dataVector.at(2);

	//strip file extension
	std::string spriteName = std::string(sprite, 0, sprite.find_last_of('.')); 

	RenderableObject *obj = &objects->at(id); //TODO handle if missing
	obj->albedoName = spriteName; //TODO handling swapping normal maps and models

}

/*
	Update the sprite, model, other rendering data of an object
*/
void RenderSystem::updateObjRender(Msg * m)
{
	SDL_Log(m->data.c_str());
	
	//id,renderable,renderType,model,normalMap,smoothness
	vector<string> objectData = split(m->data, ',');
	std::string id = objectData[0];

	RenderableObject *obj = &objects->at(id);

	if (!objectData[1].empty())
		obj->albedoName = objectData[1];
	if (!objectData[2].empty())
		obj->type = GameObject::getRenderableTypeFromName(objectData[2]);
	if (!objectData[3].empty())
		obj->modelName = objectData[3];
	if (!objectData[4].empty())
		obj->normalName = objectData[4];
	if (!objectData[5].empty())
		obj->smoothness = stof(objectData[5]);
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
	//TODO purge objects?

	state = RendererState::rendering;
	loadedLevel = atoi(m->data.c_str()); //what does this do?
	if (loadedLevel != 2) { //Reset camera when not in game
		cameraX = 0.0f;
		cameraY = 0.0f;
	}
}

