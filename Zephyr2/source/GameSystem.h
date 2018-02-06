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

extern volatile bool malive;

enum PlayerID {
	PLAYER1 = 1,
	PLAYER2 = 2,
	PLAYER3 = 3,
	PLAYER4 = 4
};

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
	
	//returns the tile distance in between two tiles on the grid;
	int getGridDistance(int aX, int aY, int bx, int bY);

	ObjectData objData;
	void removeAllGameObjects();

	const int timeFrame = 20;
	int turnStartTime = 0;

	// -1	= no level loaded
	// 0	= Main Menu
	// 1	= Settings
	// 2	= In Game
	// 3	= Game Over
	// 4	= Instructions
	int levelLoaded = -1;

	int score = 0;

	GameObject* findGameObject(std::string objectID);
	TankObject* findTankObject(std::string objectID);
	GridObject* findGridObject(std::string objectID);
	FullscreenObj* findFullscreenObject(std::string objectID);

private:
	//handler functions for the scenes in the game
	void mainMenuHandler(Msg * msg);
	void settingsMenuHandler(Msg * msg);
	void instructionMenuHandler(Msg * msg);
	void lvl1Handler(Msg * msg);
	void gameOverMenuHandler(Msg * msg);
	void executeAction(int actionNumber);
	void displayTimeLeft(int time);
	void updatePlayerHealthBar(int playerID);

	//converts grid coordinates to world coordinates
	Vector2 gridToWorldCoord(int gridX, int gridY);

	//send a message with updated object position
	void sendUpdatePosMessage(GameObject* g);

	//updates the reticle spright, and postion.
	void updateReticle();

	// The position of the marker, goes from 0 to 2, 0 being the top
	int markerPosition = 0;

	int currentAction = 0;//the current action that is being set

	vector<string> actionsToExecute; //the actions to be executed this turn. Received from the network system

	//time since the start of the current turn
	int framesSinceTurnStart = 0;

	int hexSize = 20; //"radius" of a single hexagon in the grid

	//the reticle controlled by the arrow keys. used for aiming and queing up actions
	GridObject* reticle;

	//the origin of the current action. (The Tank's expected position at the start of the action)
	GridObject* actionOrigin;

	bool validMove = false;

	//maximumNumber of actions per turn
	int maxActions = 4;
	Msg *m;
};