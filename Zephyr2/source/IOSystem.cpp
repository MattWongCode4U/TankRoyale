#include "IOSystem.h"


IOSystem::IOSystem(MessageBus* mbus) : System(mbus) {
	m = new Msg(EMPTY_MESSAGE, "");
}


IOSystem::~IOSystem() {
}

void IOSystem::startSystemLoop() {
	// used to prevent the io system from posting messages too often
	clock_t thisTime = clock();

	int currentGameTime = 0;
	while (alive) {
		thisTime = clock();
		if (thisTime  < currentGameTime) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(currentGameTime - thisTime));
		}
		currentGameTime = thisTime + timeFrame;
		checkKeyPresses();
	}
}

// checks keypresses
void IOSystem::checkKeyPresses() {
	m->data = "";
	SDL_Event gameEvent;

	while (SDL_PollEvent(&gameEvent)) {
		switch (gameEvent.type) {
		case SDL_KEYDOWN:
			switch (gameEvent.key.keysym.sym) {
			case SDLK_UP:
				m->type = UP_ARROW_PRESSED;
				if ((clock() - keyspressed["up"]) >= timebetweenPresses) {
					keyspressed["up"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_DOWN:
				m->type = DOWN_ARROW_PRESSED;
				if ((clock() - keyspressed["down"]) >= timebetweenPresses) {
					keyspressed["down"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_RIGHT:
				m->type = RIGHT_ARROW_PRESSED;
				if ((clock() - keyspressed["right"]) >= timebetweenPresses) {
					keyspressed["right"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_LEFT:
				m->type = LEFT_ARROW_PRESSED;
				if ((clock() - keyspressed["left"]) >= timebetweenPresses) {
					keyspressed["left"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_SPACE:
				m->type = SPACEBAR_PRESSED;
				if ((clock() - keyspressed["space"]) >= timebetweenPresses) {
					keyspressed["space"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_q:
				m->type = KEY_Q_PRESSED;
				if ((clock() - keyspressed["q"]) >= timebetweenPresses) {
					keyspressed["q"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_w:
				m->type = KEY_W_PRESSED;
				if ((clock() - keyspressed["w"]) >= timebetweenPresses) {
					keyspressed["w"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_e:
				m->type = KEY_E_PRESSED;
				if ((clock() - keyspressed["e"]) >= timebetweenPresses) {
					keyspressed["e"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_a:
				m->type = KEY_A_PRESSED;
				if ((clock() - keyspressed["a"]) >= timebetweenPresses) {
					keyspressed["a"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_s:
				m->type = KEY_S_PRESSED;
				if ((clock() - keyspressed["s"]) >= timebetweenPresses) {
					keyspressed["s"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_d:
				m->type = KEY_D_PRESSED;
				if ((clock() - keyspressed["d"]) >= timebetweenPresses) {
					keyspressed["d"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_z:
				m->type = TEST_KEY_PRESSED;
				if ((clock() - keyspressed["z"]) >= timebetweenPresses) {
					keyspressed["z"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			case SDLK_x:
				m->type = TEST_KEY_PRESSED;
				if ((clock() - keyspressed["x"]) >= timebetweenPresses) {
					keyspressed["x"] = clock();
					msgBus->postMessage(m, this);
				}
				break;
			}
		}
	}
}

// Currently the IO system doesn't really do anything other than check IO, there should be no messages for it to handle
// theorietically the IO system jsut posts messages
void IOSystem::handleMessage(Msg *msg) {
	// call the parent first 
	System::handleMessage(msg);
	// personal call 
	switch (msg->type) {
	case CHECK_KEY_PRESSES:
		checkKeyPresses();
		break;
	default:
		break;
	}
}