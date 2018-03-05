#pragma once
#include "SDL.h"
#include "System.h"

class RenderWrapper;

class RenderSystem : public System {
public:
	RenderSystem(MessageBus* mbus);
	~RenderSystem();

	void checkMessageQueue();
	void postMessageToQueue(Msg * msg);
	void handleMessage(Msg * msg);
	void startSystemLoop();
	void stopSystemLoop();
	void init();
	SDL_Window* GetSDLWindow();

private:
	MessageBus * _mbus;
	RenderWrapper * _wrapper;
};