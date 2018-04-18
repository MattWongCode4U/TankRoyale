#pragma once
#include "Scene.h"

/**
 * Scene: Lobby class
 * 
 * The lobby class where players can select their tank classes for battle.
 * Uses: Scene
 */
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

	//Displays an 'error' popup
	void loadNoClassSelected();
	//Removes the 'error' popup
	void unloadNoClassSelected();
};