#pragma once
#include "System.h"
//#include "GameObject.h"
#include "FullscreenObj.h"
#include "Cannonball.h"
#include "TankObject.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <thread>
#include "ObjectData.h"




extern volatile bool malive;

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
	void GameSystem::gameObjectRemoved(GameObject* g);
	std::vector<GameObject*> gameObjects;
	
	ObjectData objData;
	void removeAllGameObjects();

	const int timeFrame = 20;

	// -1	= no level loaded
	// 0	= Main Menu
	// 1	= Settings
	// 2	= In Game
	// 3	= Game Over
	// 4	= Instructions
	int levelLoaded = -1;

	int score = 0;
private:
	//handler functions for the scenes in the game
	void mainMenuHandler(Msg * msg);
	void settingsMenuHandler(Msg * msg);
	void instructionMenuHandler(Msg * msg);
	void lvl1Handler(Msg * msg);
	void gameOverMenuHandler(Msg * msg);
	void executeAction(int actionNumber);

	//converts grid coordinates to world coordinates
	Vector2 gridToWorldCoord(int gridX, int gridY);

	//send a message with updated object position
	void sendUpdatePosMessage(GameObject* g);
	// The position of the marker, goes from 0 to 2, 0 being the top
	int markerPosition = 0;

	int currentAction = 0;//the current action that is being set

	vector<string> actionsToExecute; //the actions to be executed this turn. Received from the network system

	//time since the start of the current turn
	int framesSinceTurnStart = 0;

	int hexSize = 20; //"radius" of a single hexagon in the grid

	//int reticleXGrid = 0, reticleYGrid = 0; //the hex tile the reticle is over

	//maximumNumber of actions per turn
	int maxActions = 4;
};