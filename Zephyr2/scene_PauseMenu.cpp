#include "scene_PauseMenu.h"
Scene_PauseMenu::Scene_PauseMenu(MessageBus* _mbus, GameSystem* _gs) {

};

Scene_PauseMenu::~scene_PauseMenu() {

};

//code that runs once when  scene is loaded goes here
void Scene_PauseMenu::startScene() {
	gameSystem->levelLoaded = 4;
	Msg* m = new Msg(LEVEL_LOADED, "4");
	msgBus->postMessage(m, gameSystem);
	gameSystem->addGameObjects("PauseMenu.txt");
};

//Runs once per gameloop frame
void Scene_PauseMenu::sceneUpdate() {

};

//runs whenever a message is received by GameSystem
void Scene_PauseMenu::sceneHandleMessage(Msg * msg) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");
	//GameObject* g;

	switch (msg->type) {
	case LEFT_MOUSE_BUTTON:
	{
		vector<string> objectData = split(msg->data, ',');
		INT32 x = atoi(objectData[0].c_str());
		INT32 y = atoi(objectData[1].c_str());
		INT32 width = atoi(objectData[2].c_str());
		INT32 length = atoi(objectData[3].c_str());
		x -= width / 2; y -= length / 2;
		y = -y;
		bool change = false;


		for (GameObject *g : gameSystem->gameObjects)
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("Menu_Item0") == 0)
				{
					// Loadinstructions page
					gameSystem->loadScene(INSTRUCTION_MENU);
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item1") == 0)
				{

					gameSystem->loadScene(GAMEPLAY);
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item2") == 0)
				{
					gameSystem->loadScene(SETTINGS_MENU);
					gameSystem->markerPosition = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item3") == 0)
				{
					malive = false;
					break;
				}

				/*
				// This is for the Back Button
				else if (g->id.compare("Menu_Item4") == 0)
				{
				removeAllGameObjects();
				addGameObjects("main_menu.txt");
				levelLoaded = 0;
				}*/
			}
		}
		if (change)
		{
			msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(gameSystem->levelLoaded)), gameSystem);

		}

		break;
	}
	case MOUSE_MOVE:
	{
		vector<string> objectData = split(msg->data, ',');
		INT32 x = atoi(objectData[0].c_str());
		INT32 y = atoi(objectData[1].c_str());
		INT32 width = atoi(objectData[2].c_str());
		INT32 length = atoi(objectData[3].c_str());
		x -= width / 2; y -= length / 2;
		y = -y;
		bool change = false;


		for (GameObject *g : gameSystem->gameObjects)
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("Menu_Item0") == 0 && gameSystem->markerPosition != 0)
				{
					gameSystem->markerPosition = 0; change = true;
				}
				else if (g->id.compare("Menu_Item1") == 0 && gameSystem->markerPosition != 1)
				{
					gameSystem->markerPosition = 1; change = true;
				}
				else if (g->id.compare("Menu_Item2") == 0 && gameSystem->markerPosition != 2)
				{
					gameSystem->markerPosition = 2; change = true;
				}
				else if (g->id.compare("Menu_Item3") == 0 && gameSystem->markerPosition != 3)
				{
					gameSystem->markerPosition = 3; change = true;
				}

				/*
				// This is for the back button
				else if (g->id.compare("Menu_Item4") == 0 && markerPosition != 0)
				{
				markerPosition = 4; change = true;
				}*/
			}
		}
		if (change) {
			for (int i = 0; i < 4; i++) {
				if (i == gameSystem->markerPosition) {
					msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Menu_Item" + to_string(i) + ",1,MenuItemSelected" + to_string(gameSystem->markerPosition) + ".png"), gameSystem);
				}
				else {
					msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Menu_Item" + to_string(i) + ",1,MenuItem" + to_string(i) + ".png"), gameSystem);
				}
			}
		}

		break;
	}
	default:
		break;
	}
};