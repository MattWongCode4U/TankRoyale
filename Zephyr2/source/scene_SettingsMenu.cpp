#pragma once
#include "Scene_SettingsMenu.h"
#include "GameSystem.h"

Scene_SettingsMenu::Scene_SettingsMenu(MessageBus* _mbus, GameSystem* _gs) : Scene(_mbus, _gs) {
}


Scene_SettingsMenu::~Scene_SettingsMenu() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_SettingsMenu::startScene() {
	
	gameSystem->removeAllGameObjects();
	gameSystem->addGameObjects("settings_menu.txt");
	gameSystem->levelLoaded = 1;
	Msg* m = new Msg(LEVEL_LOADED, "1");
	msgBus->postMessage(m, gameSystem);
	updateSliderPosition(3); // Default 0.6f
	
	//msgBus->postMessage(new Msg(READY_TO_START_GAME, ""), gameSystem);
}

//called every frame of the gameloop
void Scene_SettingsMenu::sceneUpdate() {
}
//called everytime a message is received by the gameSystem
void Scene_SettingsMenu::sceneHandleMessage(Msg * msg) {
		std::ostringstream oss;
		Msg* mm = new Msg(EMPTY_MESSAGE, "");
		switch (msg->type) {
		case SPACEBAR_PRESSED:
			if (gameSystem->markerPosition == 2) {
				gameSystem->loadScene(MAIN_MENU);
			}
			else if (gameSystem->markerPosition == 1) {
				// change game sound to "off"
				mm->type = AUDIO_MUTE;
				mm->data = "1";
				msgBus->postMessage(mm, gameSystem);
			}
			else if (gameSystem->markerPosition == 0) {
				// change game sound to "on"
				mm->type = AUDIO_MUTE;
				mm->data = "0";
				msgBus->postMessage(mm, gameSystem);
			}
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
					// Check if click within bounds? Or set some empty objects, for slider selection


					// This is for the Back Button
					if (g->id.compare("Back4") == 0)
					{
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						gameSystem->loadScene(MAIN_MENU);
						break;
					}
					if (g->id.compare("SoundButton1") == 0) {
						msgBus->postMessage(new Msg(AUDIO_MUTE, "0"), gameSystem);
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "SoundButton1,1,soundOn2.png,"), gameSystem);
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "SoundButton2,1,soundOff1.png,"), gameSystem);
						break;
					}
					if (g->id.compare("SoundButton2") == 0) {
						msgBus->postMessage(new Msg(AUDIO_MUTE, "1"), gameSystem);
						msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "SoundButton2,1,soundOff2.png,"), gameSystem);
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "SoundButton1,1,soundOn1.png,"), gameSystem);
						break;
					}
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
					if (g->id.compare("Back4") == 0 && gameSystem->markerPosition != 3)
					{
						gameSystem->markerPosition = 4; change = true;
					}
				}
			}

			if (change) {
				for (int i = 4; i < 5; i++) {
					if (i == gameSystem->markerPosition) {
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Back" + to_string(i) + ",1,MenuItemSelected" + to_string(gameSystem->markerPosition) + ".png"), gameSystem);
					}
					else {
						msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "Back" + to_string(i) + ",1,MenuItem" + to_string(i) + ".png"), gameSystem);
					}
				}
			}

			break;
		}

		case NUM_1_PRESSED:
			// move slider
			updateSliderPosition(1);
			msgBus->postMessage(new Msg(AUDIO_SET, "0.2"), gameSystem);
			break;
		case NUM_2_PRESSED:
			updateSliderPosition(2);
			msgBus->postMessage(new Msg(AUDIO_SET, "0.4"), gameSystem);
			break;
		case NUM_3_PRESSED:
			updateSliderPosition(3);
			msgBus->postMessage(new Msg(AUDIO_SET, "0.6"), gameSystem);
			break;
		case NUM_4_PRESSED:
			updateSliderPosition(4);
			msgBus->postMessage(new Msg(AUDIO_SET, "0.8"), gameSystem);
			break;
		case NUM_5_PRESSED:
			updateSliderPosition(5);
			msgBus->postMessage(new Msg(AUDIO_SET, "1.0"), gameSystem);
			break;
		default:
			break;
		}
}

void Scene_SettingsMenu::updateSliderPosition(int num) {
	// TODO: Get the game volume and set the position to that
	FullscreenObj *slider = gameSystem->findFullscreenObject("Sound_Select");
	if (slider == nullptr) return;
	switch (num) {
	case 1:
		slider->y = POSITION_1;
		slider->postPostionMsg();
		break;
	case 2:
		slider->y = POSITION_2;
		slider->postPostionMsg();
		break;
	case 3:
		slider->y = POSITION_3;
		slider->postPostionMsg();
		break;
	case 4:
		slider->y = POSITION_4;
		slider->postPostionMsg();
		break;
	case 5:
		slider->y = POSITION_5;
		slider->postPostionMsg();
		break;
	default:
		break;
	}
}