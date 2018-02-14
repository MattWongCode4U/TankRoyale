#pragma once
#include "Scene.h"
#include "GameSystem.h"

Scene::Scene(MessageBus* _mbus, GameSystem* _gs){
 msgBus = _mbus;
gameSystem = _gs;
}


Scene::~Scene() {
}

//called whenever the scene is loaded
void Scene::startScene() {
}
void Scene::sceneUpdate() {

 }

void Scene::sceneHandleMessage(Msg * msg) {

 }