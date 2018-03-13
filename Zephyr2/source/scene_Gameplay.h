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
	//MOVED TO THE TankObject class
	//void dealAOEDamage(int _originX, int _originY, int affectedRadius, int damage);

	//deal damage in a straight line from the origin position along an axis
	void dealLineDamage(int _originX, int _originY, int length, int axis, int damage);

	//returns true if the two points are on the same specified axis
	//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
	bool sameAxisShot(int axis, int x1, int y1, int x2, int y2, int length);
	
	//returns the axis that is shared by the 2 points. 
	//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
	//if not on any of the axis, return -1
	int onAxis(int x1, int y1, int x2, int y2, int range);

	//returns the tile distance in between two tiles on the grid;
	int getGridDistance(int aX, int aY, int bx, int bY);

	//updates the reticle spright, and postion.
	void updateReticle();

	//is the currently selected move action. Used to determine if the player is allowed to exectue selected action
	bool validMove = false;

	//the local player tank object
	TankObject* playerTank;

	//the origin of the current action. (The Tank's expected position at the start of the action)
	GridObject* actionOrigin;

	//is the game actively receiving player input? false when it game is waiting/animating
	bool gameActive = false;

	void loadPauseMenu();
	void unloadPauseMenuObjects();
};

