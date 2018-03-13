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

	//post message on the bus
	virtual void postMessageToBus(Msg* message) = 0;

	//find a gameObject by Id
	virtual GameObject* findGameObject(std::string objectID) = 0;

	//send a message with updated object position
	virtual void sendUpdatePosMessage(GameObject* g) = 0;
};