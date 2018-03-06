#pragma once
#include <windows.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <cstdlib>
#include <ctime>
#include <thread>
#include "System.h"
#include "FullscreenObj.h"
#include "GridObject.h"
#include "TankObject.h"
#include "ObjectData.h"
#include "Scene_MainMenu.h"
#include "Scene_Gameplay.h"
#include "Scene_SettingsMenu.h"
#include "Scene_InstructionsMenu.h"

extern volatile bool malive;

enum PlayerID {
	PLAYER1 = 1,
	PLAYER2 = 2,
	PLAYER3 = 3,
	PLAYER4 = 4
};

enum SceneType { MAIN_MENU, GAMEPLAY, SETTINGS_MENU, INSTRUCTION_MENU, GAME_OVER };

class GameSystem : public System {
public:
	GameSystem(MessageBus* mbus);
	~GameSystem();

	void handleMessage(Msg * msg);

	void startSystemLoop();
	//void startTestLevel();
	void addGameObjects(string fileName);
	void saveToFIle(string fileName);
	void createGameObject(GameObject* g);
	void gameObjectRemoved(GameObject* g);
	std::vector<GameObject*> gameObjects;

	//closes the old scene and opens a new one 
	void  loadScene(SceneType _scene);

	ObjectData objData;
	void removeAllGameObjects();
	void deleteGameObject(string id);
	void deleteGameObject(GameObject* go);

	//the currently loaded scene
	Scene* scene;

	const int timeFrame = 20;
	

	// -1	= no level loaded
	// 0	= Main Menu
	// 1	= Settings
	// 2	= In Game
	// 3	= Game Over
	// 4	= Instructions
	int levelLoaded = -1;

	// The position of the marker, goes from 0 to 2, 0 being the top
	int markerPosition = 0;

	int score = 0;

	std::string clientID = "defaultClientID";//the id of the client on the server

	//void setPlayerTank(std::string playerID);

	GameObject* findGameObject(std::string objectID);
	TankObject* findTankObject(std::string objectID);
	GridObject* findGridObject(std::string objectID);
	FullscreenObj* findFullscreenObject(std::string objectID);


	//the reticle controlled by the arrow keys. used for aiming and queing up actions
	GridObject* reticle;

	//Displays the amount to time left in the turn
	void displayTimeLeft(int time);

	//send a message with updated object position
	void sendUpdatePosMessage(GameObject* g);

	int currentAction = 0;//the current action that is being set

	vector<string> actionsToExecute; //the actions to be executed this turn. Received from the network system

	float hexSize = 7.1f; //"radius" of a single hexagon in the grid

	//for getting the position of the highlighted button
	GameObject* buttonHighlighted;

	//maximumNumber of actions per turn
	int maxActions = 4;


	Msg *m;


};