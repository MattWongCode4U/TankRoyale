#pragma once
#include "scene_GameOver.h"
#include "GameSystem.h"

Scene_GameOver::Scene_GameOver(MessageBus* _mbus, GameSystem* _gs): Scene(_mbus, _gs) {
}


Scene_GameOver::~Scene_GameOver() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_GameOver::startScene() {
	gameSystem->removeAllGameObjects();
	gameSystem->addGameObjects("instructions_menu.txt");//loading instruction screen for now (for testing)
	gameSystem->levelLoaded = 3;
	gameSystem->markerPosition = 0;

	Msg* m = new Msg(LEVEL_LOADED, "3");
	msgBus->postMessage(m, gameSystem);
}

//called every frame of the gameloop
void Scene_GameOver::sceneUpdate() {

}

//called everytime a message is received by the gameSystem
void Scene_GameOver::sceneHandleMessage(Msg * msg) {
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
						gameSystem->loadScene(MAIN_MENU);
						break;
					}
				}
			}
			//if (change)
			//{
			//	msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(gameSystem->levelLoaded)), gameSystem);
			//	gameSystem->setPlayerTank("player1");
			//}
		}
	}