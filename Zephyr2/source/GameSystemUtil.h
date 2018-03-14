#pragma once
#include "Msg.h"

class GameObject;
class GameSystemUtil {
public:

	//void startTestLevel();
	virtual void addGameObjects(std::string fileName) = 0;
	virtual GameObject* makeGameObject(std::string fileName) = 0;
	virtual void saveToFIle(std::string fileName) = 0;
	virtual void createGameObject(GameObject* g) = 0;
	virtual void gameObjectRemoved(GameObject* g) = 0;
	virtual void removeAllGameObjects() = 0;
	virtual void deleteGameObject(std::string id) = 0;
	virtual void deleteGameObject(GameObject* go) = 0;
	virtual std::vector<GameObject*>* getGameObjectsVector() = 0;
	virtual int getGridDistance(int aX, int aY, int bX, int bY) = 0;//gets the distance between 2 points on the hex grid

	//deal damage in a straight line from the origin position along an axis
	//virtual void dealLineDamage(int _originX, int _originY, int length, int axis, int damage);

	//returns true if the two points are on the same specified axis
	//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
	virtual bool sameAxisShot(int axis, int x1, int y1, int x2, int y2, int length);

	//returns the axis that is shared by the 2 points. 
	//axis: 0=r 1=l 2=ur 3=dl 4=ul 5=dr
	//if not on any of the axis, return -1
	virtual int onAxis(int x1, int y1, int x2, int y2, int range);

	//post message on the bus
	virtual void postMessageToBus(Msg* message) = 0;

	//find a gameObject by Id
	virtual GameObject* findGameObject(std::string objectID) = 0;

	//send a message with updated object position
	virtual void sendUpdatePosMessage(GameObject* g) = 0;
};