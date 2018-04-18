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
#include "GameSystemUtil.h"
#include "FullscreenObj.h"
#include "GridObject.h"
#include "TankObject.h"
#include "Tank_Artillery.h"
#include "Tank_Heavy.h"
#include "Tank_Scout.h"
#include "Tank_Sniper.h"
#include "Scene_MainMenu.h"
#include "Scene_Lobby.h"
#include "Scene_Gameplay.h"
#include "Scene_SettingsMenu.h"
#include "Scene_InstructionsMenu.h"
#include "scene_GameOver.h"

extern volatile bool malive;

enum PlayerID {
	PLAYER1 = 1,
	PLAYER2 = 2,
	PLAYER3 = 3,
	PLAYER4 = 4
};

//Node for the collision detection quad tree
struct quadTreeNode {
	float x; //the x position of the center of the node
	float y; //the y position of the center of the node
	float size;//width and height of the box
	bool isEmpty;//does the node contain any Objects
	bool isLeaf;
	quadTreeNode* childNodes[4] = {};//each node that isn't a leaf has 4 child nodes
	vector<GameObject*> containedObjects;//Gameobject that are in the node

	//constructor for a quad tree node
	//params: the x and y coodinates of the center of the node, and the length/width of the sides of the node
	quadTreeNode(float _x, float _y, float _size) {
		x = _x;
		y = _y;
		size = _size;
		isEmpty = true;
		isLeaf = true;
	};
};

enum SceneType { MAIN_MENU, LOBBY_MENU, GAMEPLAY, SETTINGS_MENU, INSTRUCTION_MENU, GAME_OVER };

class GameSystem : public System, GameSystemUtil {
public:
	GameSystem(MessageBus* mbus);
	~GameSystem();

	void handleMessage(Msg * msg);

	void startSystemLoop();
	//void startTestLevel();
	void addGameObjects(string fileName);
	GameObject* GameSystem::makeGameObject(string fileName);
	void saveToFIle(string fileName);
	void createGameObject(GameObject* g);
	void gameObjectRemoved(GameObject* g);
	std::vector<GameObject*> gameObjects;

	//closes the old scene and opens a new one 
	void  loadScene(SceneType _scene);
	void removeAllGameObjects();
	void deleteGameObject(string id);
	void deleteGameObject(GameObject* go);
	std::vector<GameObject*>* getGameObjectsVector();
	int getGridDistance(int aX, int aY, int bX, int bY);//gets the distance between 2 points on the hex grid

	//deal damage in a straight line from the origin position along an axis
	//void dealLineDamage(int _originX, int _originY, int length, int axis, int damage);

	//returns true if the two points are on the same specified axis
	//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
	bool sameAxisShot(int axis, int x1, int y1, int x2, int y2, int length);

	//returns the axis that is shared by the 2 points. 
	//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
	//if not on any of the axis, return -1
	int onAxis(int x1, int y1, int x2, int y2, int range);

	//post message on the bus
	void postMessageToBus(Msg* message);

	//handles the collisions using the quad tree
	void handleCollisions();

	bool checkCollision(GameObject* a, GameObject* b);
	bool checkCollision(quadTreeNode* a, GameObject* b);

	//inset the gameObject into the collision quad tree
	void insertIntoQuadTree(quadTreeNode* root, GameObject* g);

	//remove the object from the collision quad tree
	void removeFromQuadTree(quadTreeNode* root, GameObject* g);
	
	//clear all gameobjects from the collision quad tree
	void clearQuadTree(quadTreeNode* root);

	//checks if the specified object is colliding with anything, using the collision quad tree
	//returns true if there has been any collisions
	bool checkTreeCollision(quadTreeNode* root, GameObject* g);

	//the root node of the collision quad tree
	quadTreeNode* quadTreeRoot;

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
	int markerPositionPrime = 0;

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

	std::string tankClass = "heavy";
	
	Msg *m;

	std::vector<std::string> _gameOverList;

	bool win = false;

};