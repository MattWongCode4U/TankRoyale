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

//identifies the the scene, or level of the game. corresponds to a Scene_ class
enum SceneType { MAIN_MENU, LOBBY_MENU, GAMEPLAY, SETTINGS_MENU, INSTRUCTION_MENU, GAME_OVER };

//handles scene and game objects
class GameSystem : public System, GameSystemUtil {
public:
	//the minimum length of a GameSystem frame in milliseconds. 
	//used in system clocking so that fast systems don't run too fast and hog resources
	const int timeFrame = 20;

	//The gameObjects in the scene
	std::vector<GameObject*> gameObjects;

	//the currently loaded scene
	Scene* scene;

	GameSystem(MessageBus* mbus);
	~GameSystem();

	void handleMessage(Msg * msg);

	void startSystemLoop();

	//reads gameobjects from a file. instantiates them and adds them to the list of active objects
	void addGameObjects(string fileName);

	//reads a gameObject from a file instantiates it, and returns it. Doesn't actually add the gameobject to the game
	GameObject* GameSystem::makeGameObject(string fileName);

	//save the current state of the game. Not currently used by TankRoyale
	void saveToFIle(string fileName);

	//adds the GameObject passed in to the scene.
	void createGameObject(GameObject* g);

	//broadcasts a message to other systems, telling them this object is removed from play
	void gameObjectRemoved(GameObject* g);

	//closes the old scene and opens a new one 
	void  loadScene(SceneType _scene);

	//removes all objects from the scene, and broadcasts a message letting other systems know the object is removed
	void removeAllGameObjects();

	//removes a gameobject from the gameObjects vector, and deletes it
	void deleteGameObject(std::string id);
	void deleteGameObject(GameObject* go);
	std::vector<GameObject*>* getGameObjectsVector();

	//gets the distance between 2 points on the hex grid
	int getGridDistance(int aX, int aY, int bX, int bY);

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

	//check if a GameObject is colliding with another
	bool checkCollision(GameObject* a, GameObject* b);

	//check if a GameObject is colliding with a quad tree node. used to populate the quad tree
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

	//the currently loaded level. May be replaced by the SceneTpe enum
	int levelLoaded = -1;

	// The position of the marker, goes from 0 to 2, 0 being the top
	int markerPosition = 0;
	int markerPositionPrime = 0;

	//player score
	int score = 0;

	//the id of the client on the server
	std::string clientID = "defaultClientID";


	//find a GameObject by its id
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

	//the current turn action that is being set
	int currentAction = 0;

	vector<string> actionsToExecute; //the actions to be executed this turn. Received from the network system

	float hexSize = 7.1f; //"radius" of a single hexagon in the grid

	//for getting the position of the highlighted button
	GameObject* buttonHighlighted;

	//maximumNumber of actions per turn
	int maxActions = 4;

	//the player's class
	std::string tankClass = "heavy";
	
	//message pointer used in sustem message handling functions
	Msg *m;

	//State the game to be used by gameOver scene. Lists players and their ranking in the match
	std::vector<std::string> _gameOverList;

	//did the current player win the match?
	bool win = false;

};