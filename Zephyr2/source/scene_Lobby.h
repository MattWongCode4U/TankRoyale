#pragma once
#include "Scene.h"

class Scene_Lobby : public Scene {
public:
	Scene_Lobby(MessageBus* _mbus, GameSystem* _gs);
	~Scene_Lobby();

	std::string playerSelectedClass;

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	//code that runs once when  scene is loaded goes here
	void startScene();

	std::string playerSelection(int option);
	void Scene_Lobby::sendPlayerSelection();
};