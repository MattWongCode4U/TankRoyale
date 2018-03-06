#pragma once
#include "Scene.h"
#include "TankObject.h"
#include "GridObject.h"

class Scene_Gameplay : public Scene {
public:
	Scene_Gameplay(MessageBus* _mbus, GameSystem* _gs);
	~Scene_Gameplay();

	//sets the player tank pointer and sets up player
	void setPlayerTank(std::string playerID);

	//Runs once per gameloop frame
	void sceneUpdate();

	void updateActionBar(int a);

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	//code that runs once when  scene is loaded goes here
	void startScene();

	//range of the specified ability
	int range = 1;

	//time since the start of the current turn
	int framesSinceTurnStart = 0;

	int turnStartTime = 0;

	//execute the received actions
	void executeAction(int actionNumber);

	//the possible types of actions
	enum ActionTypes { MOVE, SHOOT, PASS, DEAD };

	void setActionType(ActionTypes a);

	//the current action being set up
	ActionTypes ActionType = MOVE;

	//deal aoe damage to all tiles in the affected area
	void dealAOEDamage(int _originX, int _originY, int affectedRadius, int damage);

	//returns the tile distance in between two tiles on the grid;
	int getGridDistance(int aX, int aY, int bx, int bY);

	//updates the reticle spright, and postion.
	void updateReticle();

	void updatePlayerHealthBar(std::string playerID);

	//is the currently selected move action. Used to determine if the player is allowed to exectue selected action
	bool validMove = false;

	//the local player tank object
	TankObject* playerTank;

	//the origin of the current action. (The Tank's expected position at the start of the action)
	GridObject* actionOrigin;

};

