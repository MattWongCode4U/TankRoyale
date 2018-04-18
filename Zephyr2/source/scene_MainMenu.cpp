#pragma once
#include "scene_MainMenu.h"
#include "GameSystem.h"

Scene_MainMenu::Scene_MainMenu(MessageBus* _mbus, GameSystem* _gs): Scene(_mbus, _gs) {
}


Scene_MainMenu::~Scene_MainMenu() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_MainMenu::startScene() {

	gameSystem->levelLoaded = 0;
	Msg* m = new Msg(LEVEL_LOADED, "0");
	msgBus->postMessage(m, gameSystem);
	gameSystem->addGameObjects("main_menu.txt");
}

//called every frame of the gameloop
void Scene_MainMenu::sceneUpdate() {

}

//called everytime a message is received by the gameSystem
void Scene_MainMenu::sceneHandleMessage(Msg * msg) {
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
					msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
					gameSystem->loadScene(INSTRUCTION_MENU);
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item1") == 0)
				{
					msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
					gameSystem->loadScene(LOBBY_MENU);
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item2") == 0)
				{
					msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
					gameSystem->loadScene(SETTINGS_MENU);
					gameSystem->markerPosition = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item3") == 0)
				{
					msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
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

		auto it = gameSystem->gameObjects.begin();
		for (; it != gameSystem->gameObjects.end(); ++it)
		{
			if ((x < (*it)->x + ((*it)->width / 2) && x >(*it)->x - ((*it)->width / 2)) &&
				(y < (*it)->y + ((*it)->length / 2) && y >(*it)->y - ((*it)->length / 2)))
			{
				if ((*it)->id.compare("Menu_Item0") == 0)
				{
					gameSystem->markerPosition = 0; change = true;
					break;
				}
				else if ((*it)->id.compare("Menu_Item1") == 0)
				{
					gameSystem->markerPosition = 1; change = true;
					break;
				}
				else if ((*it)->id.compare("Menu_Item2") == 0)
				{
					gameSystem->markerPosition = 2; change = true;
					break;
				}
				else if ((*it)->id.compare("Menu_Item3") == 0)
				{
					gameSystem->markerPosition = 3; change = true;
					break;
				}
			}
		}
		if (it == gameSystem->gameObjects.end() && (gameSystem->markerPosition != -1)) 
		{
			change = true;
			gameSystem->markerPosition = -1;
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


	/*
	case DOWN_ARROW_PRESSED:
	// move the marker location and let rendering know?
	markerPosition++;
	if (markerPosition > 3) {
	markerPosition = 3;
	}

	mm->type = UPDATE_OBJ_SPRITE;
	oss << "Menu_Item1,1,MenuItemSelected" << markerPosition << ".png,";
	mm->data = oss.str();
	msgBus->postMessage(mm, this);
	break;

	case UP_ARROW_PRESSED:
	// move the marker location and let rendering know?
	markerPosition--;
	if (markerPosition < 0) {
	markerPosition = 0;
	}
	markerPosition = markerPosition % 4;

	mm->type = UPDATE_OBJ_SPRITE;
	oss << "Menu_Item1,1,MenuItemSelected" << markerPosition << ".png,";
	mm->data = oss.str();
	msgBus->postMessage(mm, this);
	break;
	*/

	/*
	case SPACEBAR_PRESSED:
	if (markerPosition == 3) {
	// Exit was selected, kill main
	malive = false;
	}
	else if (markerPosition == 2) {
	// Go to settings
	removeAllGameObjects();
	addGameObjects("settings_menu.txt");
	levelLoaded = 1;
	markerPosition = 0;
	Msg* m = new Msg(LEVEL_LOADED, "1");
	msgBus->postMessage(m, this);
	}
	else if (markerPosition == 1) {
	// start the game (or go to level select?)
	// first, clear all objects
	removeAllGameObjects();

	// then, load new objects
	//addGameObjects("Level_1.txt");
	addGameObjects("prototype_level.txt");
	levelLoaded = 2;
	Msg* m = new Msg(LEVEL_LOADED, "2");
	msgBus->postMessage(m, this);
	score = 0;
	//let NetworkSystem know client is ready to start game
	msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), this);
	OutputDebugString("SENDING READY_TO_START_GAME MSG");

	setPlayerTank("player1");
	}
	else if (markerPosition == 0) {
	// instructions page
	removeAllGameObjects();
	addGameObjects("instructions_menu.txt");
	levelLoaded = 4;
	markerPosition = 0;
	Msg* m = new Msg(LEVEL_LOADED, "4");
	msgBus->postMessage(m, this);
	}
	break;
	*/

	//case NETWORK_CONNECT:
	//	gameSystem->clientID = msg->data;
	//	OutputDebugString("GS: NETWORK_CONNECT RECEIVED: \n");
	//	OutputDebugString(gameSystem->clientID.c_str());
	//	OutputDebugString("\n");
	//	break;
	default:
		break;
	}
}