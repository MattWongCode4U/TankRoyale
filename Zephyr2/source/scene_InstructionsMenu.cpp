#pragma once
#include "scene_InstructionsMenu.h"
#include "GameSystem.h"

Scene_InstructionsMenu::Scene_InstructionsMenu(MessageBus* _mbus, GameSystem* _gs): Scene(_mbus, _gs) {
}


Scene_InstructionsMenu::~Scene_InstructionsMenu() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_InstructionsMenu::startScene() {
	gameSystem->removeAllGameObjects();
	gameSystem->addGameObjects("instructions_menu.txt");
	gameSystem->levelLoaded = 4;
	gameSystem->markerPosition = 0;

	Msg* m = new Msg(LEVEL_LOADED, "4");
	msgBus->postMessage(m, gameSystem);
}

//called every frame of the gameloop
void Scene_InstructionsMenu::sceneUpdate() {

}

//called everytime a message is received by the gameSystem
void Scene_InstructionsMenu::sceneHandleMessage(Msg * msg) {
		// only one option; to go back to menu
		if (msg->type == SPACEBAR_PRESSED) {
			gameSystem->loadScene(MAIN_MENU);
		}
		if (msg->type == LEFT_MOUSE_BUTTON)
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
					// This is for the Back Button
					if (g->id.compare("Menu_Item4") == 0)
					{
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						gameSystem->loadScene(MAIN_MENU);
						break;
					}
				}
			}
		}
		if (msg->type == MOUSE_MOVE)
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
					if (g->id.compare("Menu_Item4") == 0 && gameSystem->markerPosition != 3)
					{
						gameSystem->markerPosition = 4; change = true;
					}
				}
			}

			if (change) {
				for (int i = 4; i < 5; i++) {
					if (i == gameSystem->markerPosition) {
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Menu_Item" + to_string(i) + ",1,MenuItemSelected" + to_string(gameSystem->markerPosition) + ".png"), gameSystem);
					}
					else {
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Menu_Item" + to_string(i) + ",1,MenuItem" + to_string(i) + ".png"), gameSystem);
					}
				}
			}
		}
	}