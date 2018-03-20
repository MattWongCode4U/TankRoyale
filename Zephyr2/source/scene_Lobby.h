#pragma once
#include "Scene.h"

class Scene_Lobby : public Scene {
public:
	Scene_Lobby(MessageBus* _mbus, GameSystem* _gs);
	~Scene_Lobby();

	bool gameActive = false;

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	//code that runs once when  scene is loaded goes here
	void startScene();

	void loadNoClassSelected();
	void unloadNoClassSelected();
};