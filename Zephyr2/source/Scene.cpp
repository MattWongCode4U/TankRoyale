#include "GameSystem.h"

Scene::Scene(MessageBus* _mbus, GameSystem* _gs){
 msgBus = _mbus;
gameSystem = _gs;
}


Scene::~Scene() {
}