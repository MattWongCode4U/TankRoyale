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
			std::this_thread::sleep_for(std::chrono::milliseconds(currentGameTime - thisTime));
			//std::this_thread::yield();
		}
		handleMsgQ();
		checkKeyPresses();

		currentGameTime = thisTime + timeFrame;
	}
}

// checks keypresses
void IOSystem::checkKeyPresses() {
	std::ostringstream oss;
	// Need to decide how we want to do this section - might be some issues here 
	// with regard to pressing multiple keys at the same time 
	// A possible way to do it is to send data instead of posting messages for  
	// specific keys, and then having another function somewhere that 
	// parses the data to get the proper code to the Gamesystem but that's 
	// a bit convoluted 
	if (focus)
	{
		if (GetKeyState(VK_UP) & 0x8000) {
			//OutputDebugString("Up Pressed\n"); 
			m->type = UP_ARROW_PRESSED;
			if ((clock() - keyspressed["up"]) >= timebetweenPresses) {
				keyspressed["up"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(VK_DOWN) & 0x8000) {
			//OutputDebugString("Down Pressed\n"); 
			m->type = DOWN_ARROW_PRESSED;
			if ((clock() - keyspressed["down"]) >= timebetweenPresses) {
				keyspressed["down"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(VK_SPACE) & 0x8000) {
			//OutputDebugString("Space Pressed\n"); 
			m->type = SPACEBAR_PRESSED;
			if ((clock() - keyspressed["space"]) >= timebetweenPresses) {
				keyspressed["space"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(VK_RIGHT) & 0x8000) {
			//OutputDebugString("Right Pressed\n"); 
			m->type = RIGHT_ARROW_PRESSED;
			if ((clock() - keyspressed["right"]) >= timebetweenPresses) {
				keyspressed["right"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(VK_LEFT) & 0x8000) {
			//OutputDebugString("Left Pressed\n"); 
			m->type = LEFT_ARROW_PRESSED;
			if ((clock() - keyspressed["left"]) >= timebetweenPresses) {
				keyspressed["left"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(VK_ESCAPE) & 0x8000) {
			m->type = KEY_ESC_PRESSED;
			if ((clock() - keyspressed["left"]) >= timebetweenPresses) {
				keyspressed["left"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(VK_ESCAPE) < 0) {
			m->type = KEY_ESC_RELEASED;
			if ((clock() - keyspressed["left"]) >= timebetweenPresses) {
				keyspressed["left"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('A') & 0x8000) {
			//OutputDebugString("A Pressed\n"); 
			m->type = KEY_A_PRESSED;
			if ((clock() - keyspressed["a"]) >= timebetweenPresses) {
				keyspressed["a"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('D') & 0x8000) {
			//OutputDebugString("D Pressed\n"); 
			m->type = KEY_D_PRESSED;
			if ((clock() - keyspressed["d"]) >= timebetweenPresses) {
				keyspressed["d"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('Q') & 0x8000) {
			//OutputDebugString("D Pressed\n"); 
			m->type = KEY_Q_PRESSED;
			if ((clock() - keyspressed["q"]) >= timebetweenPresses) {
				keyspressed["q"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('E') & 0x8000) {
			//OutputDebugString("D Pressed\n"); 
			m->type = KEY_E_PRESSED;
			if ((clock() - keyspressed["e"]) >= timebetweenPresses) {
				keyspressed["e"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('S') & 0x8000) {
			//OutputDebugString("D Pressed\n"); 
			m->type = KEY_S_PRESSED;
			if ((clock() - keyspressed["s"]) >= timebetweenPresses) {
				keyspressed["s"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('W') & 0x8000) {
			//OutputDebugString("D Pressed\n"); 
			m->type = KEY_W_PRESSED;
			if ((clock() - keyspressed["w"]) >= timebetweenPresses) {
				keyspressed["w"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState('Z') & 0x8000) {
			//OutputDebugString("Z Pressed\n"); 
			m->type = KEY_Z_PRESSED;
			if ((clock() - keyspressed["z"]) >= timebetweenPresses) {
				keyspressed["z"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(0x31) < 0) { // 1 key
			m->type = NUM_1_PRESSED;
			if ((clock() - keyspressed["1"]) >= timebetweenPresses) {
				keyspressed["1"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(0x32) < 0) { // 2 key
			m->type = NUM_2_PRESSED;
			if ((clock() - keyspressed["2"]) >= timebetweenPresses) {
				keyspressed["2"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(0x33) < 0) { // 3 key
			m->type = NUM_3_PRESSED;
			if ((clock() - keyspressed["3"]) >= timebetweenPresses) {
				keyspressed["3"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(0x34) < 0) { // 4 key
			m->type = NUM_4_PRESSED;
			if ((clock() - keyspressed["4"]) >= timebetweenPresses) {
				keyspressed["4"] = clock();
				msgBus->postMessage(m, this);
			}
		}

		if (GetKeyState(0x35) < 0) { // 5 key
			m->type = NUM_5_PRESSED;
			if ((clock() - keyspressed["5"]) >= timebetweenPresses) {
				keyspressed["5"] = clock();
				msgBus->postMessage(m, this);
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
	case GAINED_FOCUS:
		focus = true;
		break;
	case LOST_FOCUS:
		focus = false;
		break;
	default:
		break;
	}
}