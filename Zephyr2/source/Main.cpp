#include "Main.h"

int main(int argc, char *argv[]) {
	//////////////////////////////////////////////////////////////////
	//						Loading SDL  							//
	//////////////////////////////////////////////////////////////////
	std::cout << "\nLoading SDL\n";
	SDL_Init(SDL_INIT_EVERYTHING);

	//////////////////////////////////////////////////////////////////
	//						Loading Core							//
	//////////////////////////////////////////////////////////////////
	std::cout << "\nLoading Core\n";
	// create ONE message bus that goes to ALL the systems
	mbus = new MessageBus();

	//////////////////////////////////////////////////////////////////
	//						Loading Config							//
	//////////////////////////////////////////////////////////////////
	//int numberOfWorkerThreads = 16; // Default to 16
	std::cout << "\nLoading Config\n";
	GlobalPrefs::load();

	//////////////////////////////////////////////////////////////////
	//						SYSTEM CREATION							//
	// DO NOT START SYSTEM LOOPS IN HERE (if a loop is required)	//
	//////////////////////////////////////////////////////////////////	
	IOSystem* ios = new IOSystem(mbus);
	mbus->addSystem(ios);
	
	RenderSystem* rs = new RenderSystem(mbus);
	mbus->addSystem(rs);

	GameSystem* gs = new GameSystem(mbus);
	mbus->addSystem(gs);

	AudioSystem* aus = new AudioSystem(mbus);
	mbus->addSystem(aus);

	NetworkSystem* ns = new NetworkSystem(mbus);
	mbus->addSystem(ns);

	std::cout << "All systems created";

	// Not using this right now, move it to game system/Render/Physics later maybe
	//// Create worker thread pool
	//ctpl::thread_pool p(numberOfWorkerThreads); // number of threads in pool
	//											// for p.push usage, see the ctpl_stl.h header file

	//////////////////////////////////////////////////////////////////
	//				Start Core System Threads     					//
	//////////////////////////////////////////////////////////////////
	std::thread gameSystemThread(startGameSystem, gs);
	std::thread renderThread(startRenderSystem, rs);
	std::thread ioThread(startIOSystem, ios);
	std::thread audThread(startAudioSystem, aus);
	std::thread nsThread(startNetworkSystem, ns);
	


	//////////////////////////////////////////////////////////////////
	//						Console Loop							//
	//////////////////////////////////////////////////////////////////
	malive = true; //Move this
	clock_t thisTime;
	int currentGameTime = 0;

	// TO DO: Implement 
	while (malive) {
		thisTime = clock();
		if (thisTime  < currentGameTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(currentGameTime - thisTime));
		}
		currentGameTime = thisTime + 100;

		SDL_Event windowEvent;
		while (SDL_PollEvent(&windowEvent)) {
			if (SDL_QUIT == windowEvent.type) 
			{
				malive = false;
			}
			else if (SDL_WINDOWEVENT_FOCUS_LOST == windowEvent.window.event)
			{
				mbus->postMessage(new Msg(LOST_FOCUS, ""), NULL);
			}
			else if (SDL_WINDOWEVENT_FOCUS_GAINED == windowEvent.window.event)
			{
				mbus->postMessage(new Msg(GAINED_FOCUS, ""), NULL);
			}
			else if (SDL_MOUSEBUTTONDOWN == windowEvent.type && SDL_BUTTON_LEFT == windowEvent.button.button) 
			{
				ostringstream oss;
				INT32 x, y;
				SDL_GetWindowSize(rs->GetSDLWindow(), &x, &y);

				oss << windowEvent.button.x << "," << windowEvent.button.y << "," << x << "," << y;
				mbus->postMessage(new Msg(LEFT_MOUSE_BUTTON, oss.str()), NULL);
			}
			else if (SDL_MOUSEMOTION == windowEvent.type) {
				//OutputDebugString("Mouse Motion");
				ostringstream oss;
				INT32 x, y;
				SDL_GetWindowSize(rs->GetSDLWindow(), &x, &y);
				oss << windowEvent.button.x << "," << windowEvent.button.y << "," << x << "," << y;
				mbus->postMessage(new Msg(MOUSE_MOVE, oss.str()), NULL);
			}
		}
		//OutputDebugString("outside\n");
	}
	
	// if we're out here, that means malive was set to false.
	rs->stopSystemLoop();
	ios->alive = false;
	gs->alive = false;
	rs->alive = false;
	aus->alive = false;
	ns->alive = false;

	//////////////////////////////////////////////////////////////////
	//						Thread Joining							//
	//////////////////////////////////////////////////////////////////
	ioThread.join();
	//OutputDebugString("\nIO Ended\n");
	renderThread.join();
	//OutputDebugString("\nRT Ended\n");
	gameSystemThread.join();
	//OutputDebugString("\nGS Ended\n");
	audThread.join();
	//OutputDebugString("\nAudio Ended\n");
	nsThread.join();
	//OutputDebugString("\nNetwork Ended\n");

	//////////////////////////////////////////////////////////////////
	//						Thread Deleting							//
	//////////////////////////////////////////////////////////////////
	delete(ios);
	delete(rs);
	delete(gs);
	delete(aus);
	delete(ns);

	SDL_Quit();

	return 0;
}

// note: Must have "int id" for functinos that are to be run in worker threads
// the id is the thread ID - required for the pooling library
void postMessage(int id, Msg* msg) {
	//mbus->postMessage(msg);
}

// Required wrapper functions to thread the calls
void startIOSystem(IOSystem* s) {
	s->startSystemLoop();
}

void startRenderSystem(RenderSystem* s) {
	s->startSystemLoop();
}

void startGameSystem(GameSystem* s) {
	s->startSystemLoop();
}

void startAudioSystem(AudioSystem* s) {
	s->startSystemLoop();
}

void startNetworkSystem(NetworkSystem* s) {
	s->startSystemLoop();
}