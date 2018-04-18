#include "RenderWrapper.h"
#include "RenderPipeline.h"
#include <gtc\constants.hpp>
#include "..\GlobalPrefs.h"
#include "..\GameObject.h"

/*
RenderWrapper Constructor.
*/
RenderWrapper::RenderWrapper(RenderSystem *system) {
	this->system = system;

	//create window (now handled here)
	window = SDL_CreateWindow("Zephyr2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GlobalPrefs::windowWidth, GlobalPrefs::windowHeight, SDL_WINDOW_OPENGL);
	if (GlobalPrefs::windowFullscreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);


	//create pipeline, scene, overlay
	pipeline = new RenderPipeline(window);
	pipeline->setPostprocessingStage(GlobalPrefs::rEnablePost);
	scene = new RenderableScene();
	overlay = new RenderableOverlay();
	objects = new std::map<std::string, RenderableObject>();
}

/*
RenderWrapper destructor.
*/
RenderWrapper::~RenderWrapper() {
	delete pipeline;
	delete scene;
	delete objects;
}

/*
Initialize RenderWrapper.
Mostly sets initial values.
*/
void RenderWrapper::init() {

	//setup initial camera rig
	RenderableCamera cam;
	cam.farPlane = 1000;
	cam.nearPlane = 1;
	cam.clearColor = glm::vec3(0.25f, 0.25f, 0.25f);
	cam.viewAngle = glm::radians(60.0f);
	cam.position = glm::vec3(0, -70.0f, 3.0f);
	cam.rotation = glm::vec3(-0.8f, 0, 0);
	scene->camera = cam;

	//setup main light
	RenderableLight mainLight;
	mainLight.type = RenderableLightType::AMBIENT;
	mainLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
	mainLight.intensity = 1.0f;
	scene->lights.push_back(mainLight);
}

/*
Start of the system.
*/
void RenderWrapper::startSystemLoop() {

	//initialize self and pipeline
	init();
	pipeline->setupGLOnThread();
	pipeline->setupSceneOnThread();

	//load all resources
	state = RendererState::loading;
	pipeline->loadAllResources();

	//set state (retained from RACE, not used as much now)
	state = RendererState::rendering;
	running = true;
	


	while (running)
	{ 
		
		system->checkMessageQueue();

		//we tried switching to a pointer-based no-copy implementation, but it didn't save a meaningful amount of RAM, caused weird glitches, and was actually slower
		scene->objects.clear();
		scene->forwardObjects.clear();
		scene->billboardObjects.clear();
		overlay->elements.clear();
		for (auto el : *objects)
		{
			RenderableObject &obj = el.second;

			switch (obj.type)
			{
			case RenderableType::OBJECT3D:
				scene->objects.push_back(obj);
				break;
			case RenderableType::FORWARD3D:
			case RenderableType::BILLBOARD:
				scene->forwardObjects.push_back(obj);
				break;
			case RenderableType::OVERLAY:
				overlay->elements.push_back(obj);
				break;
			}


		}

		pipeline->doRender(scene, overlay);

		SDL_GL_SwapWindow(window);

	}

	
}

/*
Shutdown of system.
*/
void RenderWrapper::stopSystemLoop() {
	state = RendererState::unloading;
	running = false;

	pipeline->cleanupSceneOnThread();
	pipeline->cleanupGLOnThread();


	SDL_DestroyWindow(window);
}

/*
RenderWrapper handle messages from the message bus.
*/
void RenderWrapper::handleMessage(Msg *msg) {
	std::string s = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));

	switch (msg->type) {
	case EXIT_GAME:
		running = false;
		break;
	case UPDATE_OBJECT_POSITION:
		updateObjPosition(msg);
		break;
	case GO_ADDED:
		addObjectToRenderList(msg);
		break;
	case GO_REMOVED:
		removeObjectFromRenderList(msg);
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
	case CONFIG_PIPELINE:
		updatePipelineConfig(msg);
		break;
	default:
		break;
	}
}

/*
Parse message and create an id/RenderableObject pair
*/
std::pair<std::string, RenderableObject> RenderWrapper::parseObject(std::string data)
{
	std::vector<std::string> objectData = split(data, ',');

	RenderableObject obj;
	std::string id;

	//derived from old renderObject implementation
	std::string sprite, model, normal;
	float x, y, z, xRotation, yRotation, zRotation, w, l, h, smoothness;
	int frames = 1;
	int delay = 1;
	bool animateOnce = true;
	int type = (int)RenderableType::OBJECT3D;

	id = objectData[0];
	std::string spriteName = trimResourceName(objectData[1]);
	x = (float)(atof(objectData[2].c_str()));
	y = (float)(atof(objectData[3].c_str()));
	z = (float)(atof(objectData[4].c_str()));
	xRotation = (float)(atof(objectData[5].c_str()));
	yRotation = (float)(atof(objectData[6].c_str()));
	zRotation = (float)(atof(objectData[7].c_str()));
	w = (float)(atof(objectData[8].c_str()));
	l = (float)(atof(objectData[9].c_str()));
	h = (float)(atof(objectData[10].c_str()));
	frames = atoi(objectData[12].c_str());
	type = atoi(objectData[13].c_str());
	model = trimResourceName(objectData[14]);
	normal = trimResourceName(objectData[15]);
	smoothness = (float)(atof(objectData[16].c_str()));
	delay = atoi(objectData[17].c_str());
	animateOnce = atoi(objectData[18].c_str());

	//set obj data
	obj.type = (RenderableType)type;
	obj.position = glm::vec3(x, y, z);
	obj.rotation = glm::vec3(glm::radians(xRotation), glm::radians(yRotation), glm::radians(zRotation));
	obj.scale = glm::vec3(w, l, h);
	obj.modelName = model;
	obj.albedoName = spriteName;
	obj.normalName = normal;
	obj.smoothness = smoothness;
	obj.frameCount = frames;
	obj.frameDelay = delay;
	obj.animateOnce = animateOnce;
	obj.startFrame = pipeline->currentFrame();


	return std::make_pair(id, obj);
}

/*
Send message that the camera was panned.
*/
void RenderWrapper::positionUpdated() {
	Msg* m = new Msg(CAMERA_OFFSET, "");
	std::ostringstream oss;
	oss << std::to_string(scene->camera.position.x) << "," << std::to_string(scene->camera.position.y);
	m->data = oss.str();
	system->postMessageToQueue(m);
}

/*
Remove object from the list of objects to render.
*/
void RenderWrapper::removeObjectFromRenderList(Msg* m) {
	std::string keyToRemove = m->data;
	objects->erase(keyToRemove);
}

/*
Add object to the list of objects to render.
*/
void RenderWrapper::addObjectToRenderList(Msg* m) {
	auto objectPair = parseObject(m->data);
	objects->emplace(objectPair);
	//that's more like it!
}

/*
Update the position of an object given in the message.
*/
void RenderWrapper::updateObjPosition(Msg* m) {
	//SDL_Log(m->data.c_str());
	std::vector<std::string> dataVector = split(m->data, ',');

	//id,renderable,x,y,z,orientation,width,length,physEnabled,type
	std::string id = dataVector[0];

	RenderableObject *obj = &objects->at(id);

	//added sprite updating to updateObjPosition, to limit how many messages need to be sent each frame -Denis
	obj->albedoName = trimResourceName(dataVector[1].c_str());

	obj->position = glm::vec3((float)(atof(dataVector[2].c_str())), (float)(atof(dataVector[3].c_str())), (float)(atof(dataVector[4].c_str())));
	obj->rotation = glm::vec3(glm::radians((float)(atof(dataVector[5].c_str()))), glm::radians((float)(atof(dataVector[6].c_str()))), glm::radians((float)(atof(dataVector[7].c_str()))));
	obj->scale = glm::vec3((float)(atof(dataVector[8].c_str())), (float)(atof(dataVector[9].c_str())), (float)(atof(dataVector[10].c_str())));
}

/*
Update the sprite of an object (legacy)
*/
void RenderWrapper::updateObjSprite(Msg* m)
{
	std::vector<std::string> dataVector = split(m->data, ',');
	std::string id = dataVector.at(0);
	std::string sprite = dataVector.at(2);

	//strip file extension
	std::string spriteName = trimResourceName(sprite);

	RenderableObject *obj = &objects->at(id);
	obj->albedoName = spriteName;

}

/*
Update the sprite, model, other rendering data of an object
*/
void RenderWrapper::updateObjRender(Msg * m)
{

	//id,renderable,renderType,model,normalMap,smoothness,frameCount, frameDelay, animateOnce
	std::vector<std::string> objectData = split(m->data, ',');
	std::string id = objectData[0];

	RenderableObject *obj = &objects->at(id);

	if (!objectData[1].empty())
		obj->albedoName = trimResourceName(objectData[1]);
	if (!objectData[2].empty())
		obj->type = GameObject::getRenderableTypeFromName(objectData[2]);
	if (!objectData[3].empty())
		obj->modelName = trimResourceName(objectData[3]);
	if (!objectData[4].empty())
		obj->normalName = trimResourceName(objectData[4]);
	if (!objectData[5].empty())
		obj->smoothness = stof(objectData[5]);
	if (!objectData[6].empty())
		obj->frameCount = stoi(objectData[6]);
	if (!objectData[7].empty())
		obj->frameDelay = stoi(objectData[7]);
	if (!objectData[8].empty())
		obj->animateOnce = stoi(objectData[8]);
}

/*
Update the health bar on the HUD.
*/
void RenderWrapper::updateHealthHUD(Msg* m) {
	// no longer used
}

void RenderWrapper::updatePipelineConfig(Msg * m)
{
	std::vector<std::string> objectData = split(m->data, ',');

	int_fast8_t deferredEnabled = atoi(objectData[0].c_str());
	int_fast8_t forwardEnabled = atoi(objectData[1].c_str());
	int_fast8_t postprocessingEnabled = atoi(objectData[2].c_str());
	int_fast8_t overlayEnabled = atoi(objectData[3].c_str());

	if (deferredEnabled != -1)
		pipeline->setDeferredStage((bool)deferredEnabled);
	if (forwardEnabled != -1)
		pipeline->setForwardStage((bool)forwardEnabled);
	if (postprocessingEnabled != -1)
		pipeline->setPostprocessingStage((bool)postprocessingEnabled);
	if (overlayEnabled != -1)
		pipeline->setOverlayStage((bool)overlayEnabled);
}

/*
Pan the camera left.
*/
void RenderWrapper::panLeft() {
	scene->camera.position.x -= CAMERA_PAN_STEP;
}

/*
Pan the camera right.
*/
void RenderWrapper::panRight() {
	scene->camera.position.x += CAMERA_PAN_STEP;
}

/*
Pan the camera up.
*/
void RenderWrapper::panUp() {
	scene->camera.position.y += CAMERA_PAN_STEP;
}

/*
Pan the camera down.
*/
void RenderWrapper::panDown() {
	scene->camera.position.y -= CAMERA_PAN_STEP;
}

/*
Center the camera on the player.
*/
void RenderWrapper::cameraToPlayer() {
	//TODO reimplement if necessary
}

/*
Handle message when the level is loaded.
*/
void RenderWrapper::levelLoaded(Msg* m) {

	state = RendererState::rendering;
	loadedLevel = atoi(m->data.c_str()); //what does this do?
	if (loadedLevel != 2) { //Reset camera when not in game
		//scene->camera.position.x = 0.0f;
		//scene->camera.position.y = 0.0f;
	}
}
/*
Gets the Window -- Appearently needed (John)
*/
SDL_Window* RenderWrapper::GetSDLWindow()
{
	return window;
}