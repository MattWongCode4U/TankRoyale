#pragma once
#include "Scene.h"

#define POSITION_1 135
#define POSITION_2 75
#define POSITION_3 15
#define POSITION_4 -45
#define POSITION_5 -115

class Scene_SettingsMenu : public Scene {
public:
	Scene_SettingsMenu(MessageBus* _mbus, GameSystem* _gs);
	~Scene_SettingsMenu();

	//Runs once per gameloop frame
	void sceneUpdate();

	//runs whenever a message is received by GameSystem
	void sceneHandleMessage(Msg * msg);

	//code that runs once when  scene is loaded goes here
	void startScene();

	// Sets the slider position based on the user's last selection
	void updateSliderPosition(int num);
};

