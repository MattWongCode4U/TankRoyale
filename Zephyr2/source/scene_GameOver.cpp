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
	gameSystem->addGameObjects("gameover_scene.txt");//loading instruction screen for now (for testing)
	gameSystem->levelLoaded = 3;
	gameSystem->markerPosition = 0;

	Msg* m = new Msg(LEVEL_LOADED, "3");
	msgBus->postMessage(m, gameSystem);

	PopulateTable(gameSystem->_gameOverList);
}

//called every frame of the gameloop
void Scene_GameOver::sceneUpdate() 
{

}

//called everytime a message is received by the gameSystem
void Scene_GameOver::sceneHandleMessage(Msg * msg) 
{
	// only one option; to go back to menu
	switch (msg->type)
	{
	case SPACEBAR_PRESSED:
		gameSystem->loadScene(MAIN_MENU);
		break;
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
				if (g->id.compare("GameOverMenuItem3") == 0)
				{
					// Load main menu
					msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
					gameSystem->loadScene(MAIN_MENU);
					change = true;
					break;
				}
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
		auto it = gameSystem->gameObjects.begin();
		for (; it != gameSystem->gameObjects.end(); ++it)
		{
			if ((x < (*it)->x + ((*it)->width / 2) && x >(*it)->x - ((*it)->width / 2)) &&
				(y < (*it)->y + ((*it)->length / 2) && y >(*it)->y - ((*it)->length / 2)))
			{
				if ((*it)->id.compare("GameOverMenuItem3") == 0)
				{
					gameSystem->markerPosition = 3; change = true;
					break;
				}
			}
		}
		if (it == gameSystem->gameObjects.end() && gameSystem->markerPosition == 3)
		{
			change = true;
			gameSystem->markerPosition = 0;
		}
		
		if (change) {
			if (gameSystem->markerPosition == 3)
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverMenuItem" + to_string(3) + ",1,MenuItemSelected" + to_string(gameSystem->markerPosition) + ".png"), gameSystem);
			else
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverMenuItem" + to_string(3) + ",1,MenuItem" + to_string(3) + ".png"), gameSystem);
		}
		break;
	}
	}
}

void Scene_GameOver::PopulateTable(const std::vector<std::string> & vec)
{
	size_t count = 4, which = 1;
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		if (it->compare("player1") == 0) which = 1;
		else if(it->compare("player2") == 0) which = 2;
		else if (it->compare("player3") == 0) which = 3;
		else if (it->compare("player4") == 0) which = 4;

		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverPlace" + to_string(count) + ",1,player" + to_string(which) + "_tag" + ".png"), gameSystem);
		count--;
	}
}