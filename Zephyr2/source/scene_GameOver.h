#pragma once
#include "Scene.h"

class Scene_GameOver : public Scene {
public:
	Scene_GameOver(MessageBus* _mbus, GameSystem* _gs);
	~Scene_GameOver();

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	void PopulateTable(const std::vector<std::string> & vec);

	//code that runs once when  scene is loaded goes here
	void startScene();
};

