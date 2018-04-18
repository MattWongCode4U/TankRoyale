#pragma once
#include "SDL.h"
#include "System.h"

//forward declaration key to the PIMPL pattern
class RenderWrapper;

/*
RenderSystem actually has no functionality; 
*/
class RenderSystem : public System {
public:
	RenderSystem(MessageBus* mbus);
	~RenderSystem();

	//these are real methods, but pretty much just pass through
	void checkMessageQueue();
	void postMessageToQueue(Msg * msg);
	void handleMessage(Msg * msg);
	void startSystemLoop();
	void stopSystemLoop();
	void init();
	SDL_Window* GetSDLWindow();

private:
	MessageBus * _mbus; //we need to store an upward reference to the message bus to handle messages
	RenderWrapper * _wrapper; //this is the forward declared wrapper above
};