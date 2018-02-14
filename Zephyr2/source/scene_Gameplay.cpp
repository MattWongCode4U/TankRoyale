#pragma once
#include "Scene_Gameplay.h"
#include "GameSystem.h"

Scene_Gameplay::Scene_Gameplay(MessageBus* _mbus, GameSystem* _gs) : Scene(_mbus, _gs) {
}


Scene_Gameplay::~Scene_Gameplay() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_Gameplay::startScene() {
	Msg* m = new Msg(LEVEL_LOADED, "0");
	msgBus->postMessage(m, gameSystem);
	gameSystem->addGameObjects("main_menu.txt");
}

//called every frame of the gameloop
void Scene_Gameplay::sceneUpdate() {

}

//called everytime a message is received by the gameSystem
void Scene_Gameplay::sceneHandleMessage(Msg * msg) {

}