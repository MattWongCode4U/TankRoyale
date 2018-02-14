#pragma once
#include "FullscreenObj.h"
#include "GridObject.h"
#include "TankObject.h"
#include "ObjectData.h"

class GameSystem;
class Scene
{
public:
	std::vector<GameObject*> sceneObjects;
	std::string saveFile = "main_menu.txt";
	GameSystem* parentGameSystem;

	Scene(GameSystem* parentGameSystem, MessageBus* mbus);
	~Scene();

	//System::System(MessageBus* mbus) {
	//	msgBus = mbus;
	//}

	virtual void sceneUpdate();
	virtual void handleMessages(Msg * msg);
	void addGameObjects(string fileName);
	void createGameObject(GameObject* g);
	void removeAllGameObjects();

	ObjectData objData;

};

