#pragma once
#include "Scene.h"
/*
* A scene transitioned to when the player is killed or when the player wins.
* It allocates the needed scene data and the leaderboard.
* Redirects to main menu when retreat button is clicked.
*/
class Scene_GameOver : public Scene {
public:
	Scene_GameOver(MessageBus* _mbus, GameSystem* _gs);
	~Scene_GameOver();

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	// Populates the leaderboard table
	void PopulateTable(const std::vector<std::string> & vec);

	//code that runs once when  scene is loaded goes here
	void startScene();
};

