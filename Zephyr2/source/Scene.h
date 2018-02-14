#pragma once
#include "MessageBus.h"
class GameSystem;
class Scene {
public:
	MessageBus* msgBus;
	GameSystem* gameSystem;

	Scene(MessageBus* _mbus, GameSystem* _gs);
	~Scene();

	//functions to be implemented in the child scenes.
	virtual void sceneUpdate() {};
	virtual void sceneHandleMessage(Msg * msg) {};
	virtual void startScene() {};
};