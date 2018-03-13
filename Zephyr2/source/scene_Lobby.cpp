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
					if (g->id.compare("Option0") == 0) {
						// tank 1
						gameSystem->tankClass = "scout";
						break;
					}
					else if (g->id.compare("Option1") == 0) {
						// tank 2
						gameSystem->tankClass = "sniper";
						break;
					}
					else if (g->id.compare("Option2") == 0) {
						// tank 3
						gameSystem->tankClass = "heavy";
						break;
					}
					else if (g->id.compare("Option3") == 0) {
						// tank 4
						gameSystem->tankClass = "battle"; // regular tank?
						break;
					}
					else if (g->id.compare("SelectButton") == 0) {
						// Select
						// Load main menu
						if (gameSystem->tankClass != "" 
							|| gameSystem->tankClass != NULL) {
							gameSystem->loadScene(GAMEPLAY);
							change = true;
						} else {
							// TODO: No tank selected
							gameActive = false;
							loadNoClassSelected();
						}
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
						unloadNoClassSelected();
						gameActive = true;
					}
				}
			}
			break;
		}
	}
};

void scene_Lobby::loadNoClassSelected() {
	gameSystem->addGameObjects("no_class_selected.txt");
};

void scene_Lobby::unloadNoClassSelected() {
	gameSystem->deleteGameObject(gameSystem->findFullscreenObject("NoClassButton"));
};