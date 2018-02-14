#pragma once
#include "Scene.h"
class Scene_MainMenu :
	public Scene
{
public:
	Scene_MainMenu(GameSystem* _parentGameSystem);
	~Scene_MainMenu();

	void sceneUpdate();
	void handleMessages(Msg * msg);
};

