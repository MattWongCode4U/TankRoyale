#pragma once
#include "System.h"
#include <iostream>
#include "RenderSystem.h"
#include "GameSystem.h"
#include "IOSystem.h"
#include "AudioSystem.h"
#include "NetworkSystem.h"
#include <time.h>



MessageBus* mbus;
volatile bool malive;

int main(int argc, char *argv[]);

void postMessage(int id, Msg* msg);
void startIOSystem(IOSystem* s);
void startRenderSystem(RenderSystem* s);
void startGameSystem(GameSystem* s);
void startAudioSystem(AudioSystem* s);
void startNetworkSystem(NetworkSystem* s);


