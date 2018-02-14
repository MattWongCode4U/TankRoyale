#include "Scene_MainMenu.h"



Scene_MainMenu::Scene_MainMenu(GameSystem* _parentGameSystem) : Scene(_parentGameSystem)
{
}


Scene_MainMenu::~Scene_MainMenu()
{
}

void Scene_MainMenu::sceneUpdate() {

}
void Scene_MainMenu::handleMessages(Msg * msg) {

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


		for (GameObject *g : sceneObjects)
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("Menu_Item0") == 0)
				{
					// instructions page
					removeAllGameObjects();
					addGameObjects("instructions_menu.txt");
					levelLoaded = 4;
					markerPosition = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item1") == 0)
				{
					// start the game (or go to level select?)
					// first, clear all objects
					removeAllGameObjects();

					// then, load new objects
					//addGameObjects("Level_1.txt");
					addGameObjects("prototype_level.txt");
					levelLoaded = 2;
					score = 0;
					change = true;
					break;
				}
				else if (g->id.compare("Menu_Item2") == 0)
				{
					// Go to settings
					removeAllGameObjects();
					addGameObjects("settings_menu.txt");
					levelLoaded = 1;
					markerPosition = 0;
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
			msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(levelLoaded)), this);
			if (levelLoaded == 2) {
				msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), this);
			}
			setPlayerTank("player1");
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


		for (GameObject *g : sceneObjects)
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("Menu_Item0") == 0 && markerPosition != 0)
				{
					markerPosition = 0; change = true;
				}
				else if (g->id.compare("Menu_Item1") == 0 && markerPosition != 1)
				{
					markerPosition = 1; change = true;
				}
				else if (g->id.compare("Menu_Item2") == 0 && markerPosition != 2)
				{
					markerPosition = 2; change = true;
				}
				else if (g->id.compare("Menu_Item3") == 0 && markerPosition != 3)
				{
					markerPosition = 3; change = true;
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
				if (i == markerPosition) {
					msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Menu_Item" + to_string(i) + ",1,MenuItemSelected" + to_string(markerPosition) + ".png"), this);
				}
				else {
					msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Menu_Item" + to_string(i) + ",1,MenuItem" + to_string(i) + ".png"), this);
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

	case NETWORK_CONNECT:
		clientID = msg->data;
		OutputDebugString("GS: NETWORK_CONNECT RECEIVED: \n");
		OutputDebugString(clientID.c_str());
		OutputDebugString("\n");
		break;
	default:
		break;
	}
}
