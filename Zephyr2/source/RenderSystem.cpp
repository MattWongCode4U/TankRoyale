#include "RenderSystem.h"
#include "renderer\RenderWrapper.h"

RenderSystem::RenderSystem(MessageBus * mbus) : System(mbus)
{
	_wrapper = new RenderWrapper(this);
}

RenderSystem::~RenderSystem()
{
	delete _wrapper;
	_wrapper = nullptr;
}

void RenderSystem::handleMessage(Msg * msg)
{
	System::handleMessage(msg);
	_wrapper->handleMessage(msg);
}

void RenderSystem::checkMessageQueue()
{
	System::handleMsgQ();
}

void RenderSystem::postMessageToQueue(Msg * msg)
{
	msgBus->postMessage(msg, this);
}

void RenderSystem::startSystemLoop()
{
	_wrapper->startSystemLoop();
}

void RenderSystem::stopSystemLoop()
{
	_wrapper->stopSystemLoop();
}

void RenderSystem::init()
{
	_wrapper->init();
}

SDL_Window * RenderSystem::GetSDLWindow()
{
	return _wrapper->GetSDLWindow();
}
