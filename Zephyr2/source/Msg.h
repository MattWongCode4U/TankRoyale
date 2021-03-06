#pragma once
#include "Util.h"

// list all enums
enum MSG_TYPE {
	EMPTY_MESSAGE,

	// Temporary Render System
	RENDER_FRAME_TEST, UPDATE_TEST_OBJ, OBJ_TEST_MSG,

	// Render System
	UPDATE_OBJ_SPRITE,
	UPDATE_OBJ_RENDER, //id,renderable,renderType,model,normalMap,smoothness, frameCount, frameDelay

	// Physics System
	UPDATE_OBJECT_POSITION, //id,renderable,x,y,z,orientation,width,length,height,physEnabled,type
	DATA_RESPONSE, PASS_WIND,

	GO_COLLISION,

	// Game System
	GO_REMOVED, GO_ADDED, LOAD_LEVEL, LOAD_MENU, EXIT_GAME, ITEM_SELECTED, CHANGE_MAST, CHANGE_RUDDER, REQUEST_DATA, LEVEL_LOADED, GET_WIND_DIR, GET_WIND_POW, UPDATE_HP_BAR,
	SHIP_SANK, //id,xpos,ypos

	PLAYER_SELECT, PASS_LEADERBOARD,

	// IO System 
	CHECK_KEY_PRESSES, TEST_KEY_PRESSED,
	UP_ARROW_PRESSED, DOWN_ARROW_PRESSED, RIGHT_ARROW_PRESSED, LEFT_ARROW_PRESSED, SPACEBAR_PRESSED, KEY_A_PRESSED, KEY_D_PRESSED, KEY_S_PRESSED, KEY_W_PRESSED, KEY_Q_PRESSED, KEY_E_PRESSED, KEY_Z_PRESSED, KEY_ESC_PRESSED, KEY_ESC_RELEASED, NUM_1_PRESSED, NUM_2_PRESSED, NUM_3_PRESSED, NUM_4_PRESSED, NUM_5_PRESSED,
	LEFT_MOUSE_BUTTON, MOUSE_MOVE, GAINED_FOCUS, LOST_FOCUS,
	// Audio System
	AUDIO_MUTE, AUDIO_DOWN, AUDIO_UP, AUDIO_SET, CHANGE_SOUNDTRACK,

	// TankRoyale Audio Messages
	REGULAR_SHOT_SOUND, SNIPER_SHOT_SOUND, ARTILLERY_SHOT_SOUND, MOVEMENT_SOUND, // Game Shots
	BUTTON_SELECT_SOUND, MAIN_MENU_SOUND, GAME_MENU_SOUND,

	// AI system
	SCORED_HIT, //the object scored a hit on an enemy (used in AI learning) shooterId,shotObjectID, projectileID
	SHOOT_CANNON, //id,direction

	// Render System
	CAMERA_OFFSET, //the position of the camera in world space :   xposition,yposition
	CONFIG_PIPELINE, //configure the pipeline (-1 for no change, 0 for off, 1 for on): deferredPass, forwardPass, postprocessing, overlay

	// Network System
	// Format: 
	// NETWORK_S_ACTION for sending to network
	// NETWOR_R_ACTION is what network system will send to our game system when it receives an action
	NETWORK_TURN_BROADCAST,
	NETWORK_R_IDLE, NETWORK_S_IDLE,
	NETWORK_S_ACTION, //playerID,actionName,actionNumber,targetX,targetY
	NETWORK_R_ACTION,
	NETWORK_R_PING,
	NETWORK_CONNECT,
	READY_TO_START_GAME, // setn from main menuwhen player hits "start game"
	NETWORK_R_GAMESTART_OK,
	NETWORK_R_START_TURN,
	NETWORK_S_ANIMATIONS
};

class Msg {
public:
	Msg(MSG_TYPE t, std::string d);
	Msg(MSG_TYPE t);
	~Msg();

	MSG_TYPE type;
	std::string data;

};

