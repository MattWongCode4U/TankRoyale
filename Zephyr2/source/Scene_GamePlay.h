#pragma once
#include "Scene.h"
class Scene_GamePlay :
	public Scene
{
public:
	Scene_GamePlay();
	~Scene_GamePlay();

	void sceneUpdate();
	void handleMessages(Msg * msg);
};

