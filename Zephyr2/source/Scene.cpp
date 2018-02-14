#pragma once
#include "Scene.h"
#include "GameSystem.h"

Scene::Scene(MessageBus* _mbus, GameSystem* _gs){
 msgBus = _mbus;
gameSystem = _gs;
}


Scene::~Scene() {
}

//These virtual functions are redefined in the Scene class children
//void Scene::startScene() {
//	OutputDebugString("SCENE PARENT: STARTING SCENE");
//}
//void Scene::sceneUpdate() {
// }
//
//void Scene::sceneHandleMessage(Msg * msg) {
// }