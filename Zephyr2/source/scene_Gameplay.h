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
	enum ActionTypes { MOVE, SHOOT, PASS, DEAD, ROTATEPOS, ROTATENEG };

	void setActionType(ActionTypes a);

	//the current action being set up
	ActionTypes ActionType = MOVE;

	GameObject* actionIndicator;
	GameObject* reticle;

	//updates the reticle spright, and postion.
	void updateReticle();

	void checkAOEReticle();

	//sends a message to network system with the specified action
	void sendNetworkActionMsg(ActionTypes actionType);

	//the action point cost of the current action. -1 if action not allowed
	//bool validMove = false;
	int moveCost = -1;


	//the local player tank object
	TankObject* playerTank;

	//the origin of the current action. (The Tank's expected position at the start of the action)
	GridObject* actionOrigin;

	//is the game actively receiving player input? false when it game is waiting/animating
	bool gameActive = false;

	void loadPauseMenu();
	void unloadPauseMenuObjects();
};

