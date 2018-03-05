#pragma once
#include "Scene.h"

class Scene_PauseMenu : public Scene {
public:
	Scene_PauseMenu(MessageBus* _mbus, GameSystem* _gs);
	~Scene_PauseMenu();

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	//code that runs once when  scene is loaded goes here
	void startScene();
};