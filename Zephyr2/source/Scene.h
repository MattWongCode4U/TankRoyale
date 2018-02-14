#pragma once
#include "MessageBus.h"
class GameSystem;
class Scene {
public:
	MessageBus* msgBus;
	GameSystem* gameSystem;

	Scene(MessageBus* _mbus, GameSystem* _gs);
	~Scene();

	virtual void sceneUpdate();
	//virtual void sceneHandeMessage();
	//void sceneUpdate();
	virtual void sceneHandleMessage(Msg * msg);

	//code that runs once when ascene is loaded goes here
	virtual void startScene();
};