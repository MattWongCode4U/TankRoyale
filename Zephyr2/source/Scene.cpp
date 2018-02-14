#include "Scene.h"



Scene::Scene(GameSystem* _parentGameSystem, MessageBus* mbus){
	parentGameSystem = _parentGameSystem;
}



Scene::~Scene()
{
}

//reads gameobjects from a file. instantiates them and adds them to the list of active objects
void Scene::addGameObjects(string fileName) {

	std::string data = openFileFromAsset(fileName, ASSET_TYPE::DATA, true);

	vector<string> splitDataVector = split(data, ';');//split gameobjects by

	GameObject* g; //new gameobject to be created
				   //loop through objects read in from file
	for (int j = 0; j < splitDataVector.size(); j++) {

		vector<string> splitObjData = split(splitDataVector[j], ',');

		std::map<std::string, std::string> gameObjDataMap;
		//loop through elements of each GameObject and add them to the object parameter map
		for (int i = 0; i < splitObjData.size(); i++) {
			vector<string> keyValue = split(splitObjData[i], ':');
			gameObjDataMap[keyValue[0]] = keyValue[1];
		}

		//gets the gameObject type
		string gameObjectType = gameObjDataMap.find("gameObjectType")->second;
		g = NULL;
		//just hard coded else ifs for now... should probably make retreive available classes automatically <- Did some research, cpp doesn't support reflection (Hank)
		if (gameObjectType.compare("GridObject") == 0) {
			g = new GridObject(gameObjDataMap, &objData);
			//OutputDebugString(g->toString().c_str());
		}
		else if (gameObjectType.compare("GameObject") == 0) {
			g = new GameObject(gameObjDataMap, &objData);
		}
		else if (gameObjectType.compare("FullscreenObj") == 0) {
			g = new FullscreenObj(gameObjDataMap, &objData);
		}
		else if (gameObjectType.compare("TankObject") == 0) {
			g = new TankObject(gameObjDataMap, &objData);
		}

		if (g != NULL) {
			createGameObject(g);
		}
	}
}

// This function adds a created game object to the main list, and posts a message to the render
// and physics systems so that they can add it to their list as well
void Scene::createGameObject(GameObject* g) {
	//check if object id already exists
	for (GameObject* obj : sceneObjects) {
		if (g->id == obj->id) {
			return;
		}
	}
	sceneObjects.push_back(g);
	std::ostringstream oss; //id,renderable,x,y,z,orientation,width,length,physicsEnabled,objectType,imageFrames,renderType,model,normalMap,smoothness
	oss << g->id << ','
		<< g->renderable << ','
		<< g->x << ',' << g->y << ',' << g->z << ','
		<< g->orientation << ','
		<< g->width << ',' << g->length << ','
		//<< g->physicsEnabled << ','
		<< g->getObjectType() << ','
		<< g->imageFrames << ","
		<< (int)g->renderType << ","
		<< g->model << ","
		<< g->normalMap << ","
		<< g->smoothness;
	//<< g->renderable;
	// maybe add the rest of the variables into the oss as well, but can decide later depending on
	// what physics needs

	msgBus->postMessage(new Msg(GO_ADDED, oss.str()), this);
}

// wrapper that removes all game objects by sending a message for each object.
// the other possible way to do this is by using a functiona nd then posting a specific
// message to the render system, but i don't wanna touch the render system right now
void Scene::removeAllGameObjects() {
	for (GameObject* go : sceneObjects) {
		gameObjectRemoved(go);
		//scene->sceneObjects.erase(remove(scene->sceneObjects.begin(), scene->sceneObjects.end(), go), scene->sceneObjects.end());
	}

	sceneObjects.clear();
}
