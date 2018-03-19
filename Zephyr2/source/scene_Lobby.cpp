#include "scene_Lobby.h"
#include "GameSystem.h"
Scene_Lobby::Scene_Lobby(MessageBus* _mbus, GameSystem* _gs): Scene(_mbus, _gs) {

};

Scene_Lobby::~Scene_Lobby() {

};

//code that runs once when  scene is loaded goes here
void Scene_Lobby::startScene() {
	gameSystem->levelLoaded = 4;
	Msg* m = new Msg(LEVEL_LOADED, "4");
	msgBus->postMessage(m, gameSystem);
	gameSystem->addGameObjects("lobby_menu.txt");
	gameSystem->tankClass = "";
	gameActive = true;
};

//Runs once per gameloop frame
void Scene_Lobby::sceneUpdate() {

};

//runs whenever a message is received by GameSystem
void Scene_Lobby::sceneHandleMessage(Msg * msg) {
	std::ostringstream oss;
	Msg* mm = new Msg(EMPTY_MESSAGE, "");
	//GameObject* g;
	if(gameActive) {
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
			for (GameObject *g : gameSystem->gameObjects) {
				if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
					(y < g->y + (g->length / 2) && y > g->y - (g->length / 2))) {
					if (g->id.compare("Option0") == 0 && gameSystem->markerPositionPrime != -1) {
						// tank 1
						gameSystem->markerPositionPrime = 0; change = true;
						gameSystem->tankClass = "scout";
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						OutputDebugString("scout SELECTED\n");
						break;
					}
					else if (g->id.compare("Option1") == 0 && gameSystem->markerPositionPrime != 1) {
						// tank 2
						gameSystem->markerPositionPrime = 1; change = true;
						gameSystem->tankClass = "sniper";
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						OutputDebugString("sniper SELECTED\n");
						break;
					}
					else if (g->id.compare("Option2") == 0 && gameSystem->markerPositionPrime != 2) {
						// tank 3
						gameSystem->markerPositionPrime = 2; change = true;
						gameSystem->tankClass = "heavy";
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						OutputDebugString("heavy SELECTED\n");
						break;
					}
					else if (g->id.compare("Option3") == 0 && gameSystem->markerPositionPrime != 3) {
						// tank 4
						gameSystem->markerPositionPrime = 3; change = true;
						gameSystem->tankClass = "artillery"; 
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						OutputDebugString("artillery SELECTED\n");
						break;
					}
					else if (g->id.compare("Option4") == 0) {
						// Back to menu
						gameSystem->loadScene(MAIN_MENU);
						return;
						//change = true;
						//break;
					}
					else if (g->id.compare("Option5") == 0) {
						// Select
						// Load main menu
						if (gameSystem->tankClass != "") {
							msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
							gameSystem->loadScene(GAMEPLAY);
							return;
							//change = true;
						} else {
							msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
							gameActive = false;
							loadNoClassSelected();
						}
						break;
					}
					else if (g->id.compare("BackButton") == 0) {
						// Back to menu
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						gameSystem->loadScene(MAIN_MENU);
						change = true;
						break;
					}
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
					if (g->id.compare("Option4") == 0 && gameSystem->markerPosition != 3)
					{
						gameSystem->markerPosition = 4; change = true;
					}
					else if (g->id.compare("Option5") == 0 && gameSystem->markerPosition != 5)
					{
						gameSystem->markerPosition = 5; change = true;
					}
				}
			}

			if (change) {
				for (int i = 4; i < 6; i++) {
					if (i == gameSystem->markerPosition) {
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Option" + to_string(i) + ",1,MenuItemSelected" + to_string(gameSystem->markerPosition) + ".png"), gameSystem);
					}
					else {
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Option" + to_string(i) + ",1,MenuItem" + to_string(i) + ".png"), gameSystem);
					}
				}
			}

			break;
		}
		default:
			break;
		}
	} else {
		switch (msg->type) {
			case LEFT_MOUSE_BUTTON:
			vector<string> objectData = split(msg->data, ',');
			INT32 x = atoi(objectData[0].c_str());
			INT32 y = atoi(objectData[1].c_str());
			INT32 width = atoi(objectData[2].c_str());
			INT32 length = atoi(objectData[3].c_str());
			x -= width / 2; y -= length / 2;
			y = -y;
			bool change = false;
			for (GameObject *g : gameSystem->gameObjects) {
				if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
					(y < g->y + (g->length / 2) && y > g->y - (g->length / 2))) {
					if (g->id.compare("NoClassButton") == 0) {
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						unloadNoClassSelected();
						gameActive = true;
					}
				}
			}
			break;
		}
	}
};

void Scene_Lobby::loadNoClassSelected() {
	gameSystem->addGameObjects("no_class_selected.txt");
};

void Scene_Lobby::unloadNoClassSelected() {
	gameSystem->deleteGameObject(gameSystem->findFullscreenObject("NoClassButton"));
};