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
};

//Runs once per gameloop frame
void Scene_Lobby::sceneUpdate() {

};

//runs whenever a message is received by GameSystem
void Scene_Lobby::sceneHandleMessage(Msg * msg) {
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


		for (GameObject *g : gameSystem->gameObjects) {
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2))) {
				if (g->id.compare("Option0") == 0) {
					// tank 1
					break;
				}
				else if (g->id.compare("Option1") == 0) {
					// tank 2
					break;
				}
				else if (g->id.compare("Option2") == 0) {
					// tank 3
					break;
				}
				else if (g->id.compare("Option3") == 0) {
					// tank 4
					break;
				}
				else if (g->id.compare("SelectButton") == 0) {
					// Select
					// Load main menu
					gameSystem->loadScene(GAMEPLAY);
					change = true;
					break;
				}
				else if (g->id.compare("BackButton") == 0) {
					// Back to menu
					gameSystem->loadScene(MAIN_MENU);
					change = true;
					break;
				}
			}
		}
		if (change) msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(gameSystem->levelLoaded)), gameSystem);
		break;
	}
	default:
		break;
	}
};

std::string Scene_Lobby::playerSelection(int option) {
	switch (option) {
		case 0:
			// tank 1
		break;
		case 1:
			// tank 2
		break;
		case 2:
			// tank 3
		break;
		case 3:
			// tank 4
		break;
		default:
			// option 1 by default if nothing selected
		break;
	}
	return "";
}

void Scene_Lobby::sendPlayerSelection() {
	// playerSelectedClass is the string for the game object selected
	Msg* m = new Msg(PLAYER_SELECT, playerSelectedClass);
	msgBus->postMessage(m, gameSystem);
}