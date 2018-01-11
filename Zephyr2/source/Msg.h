#pragma once
#include "Util.h"

// list all enums
enum MSG_TYPE {
	EMPTY_MESSAGE,

	// Temporary Render System
	RENDER_FRAME_TEST, UPDATE_TEST_OBJ, OBJ_TEST_MSG,

	// Render System
	UPDATE_OBJ_SPRITE,

	// Physics System
	UPDATE_OBJECT_POSITION, //id,renderable,x,y,z,orientation,width,length,physEnabled,type
	DATA_RESPONSE, PASS_WIND,

	GO_COLLISION,

	// Game System
	GO_REMOVED, GO_ADDED, LOAD_LEVEL, LOAD_MENU, EXIT_GAME, ITEM_SELECTED, CHANGE_MAST, CHANGE_RUDDER, REQUEST_DATA, LEVEL_LOADED, GET_WIND_DIR, GET_WIND_POW, UPDATE_HP_BAR,
	SHIP_SANK, //id,xpos,ypos

	// IO System 
	CHECK_KEY_PRESSES, TEST_KEY_PRESSED,
	UP_ARROW_PRESSED, DOWN_ARROW_PRESSED, RIGHT_ARROW_PRESSED, LEFT_ARROW_PRESSED, SPACEBAR_PRESSED, KEY_A_PRESSED, KEY_D_PRESSED, KEY_S_PRESSED, KEY_W_PRESSED, KEY_Q_PRESSED, KEY_E_PRESSED,

	// Audio System
	AUDIO_MUTE, CHANGE_SOUNDTRACK,

	// AI system
	SCORED_HIT, //the object scored a hit on an enemy (used in AI learning) shooterId,shotObjectID, projectileID
	SHOOT_CANNON, //id,direction

	// Render System
	CAMERA_OFFSET, //the position of the camera in world space :   xposition,yposition

	// Network System
	// Format: 
	// NETWORK_S_ACTION for sending to network
	// NETWOR_R_ACTION is what network system will send to our game system when it receives an action
	NETWORK_TURN_BROADCAST,
	NETWORK_R_IDLE, NETWORK_S_IDLE,
	NETWORK_S_ACTION,//playerID,actionName,actionNumber,targetX,targetY
	NETWORK_R_ACTION
};

class Msg {
public:
	Msg(MSG_TYPE t, std::string d);
	~Msg();

	MSG_TYPE type;
	std::string data;

};

