#pragma once
#include "scene_GameOver.h"
#include "GameSystem.h"

Scene_GameOver::Scene_GameOver(MessageBus* _mbus, GameSystem* _gs): Scene(_mbus, _gs) {
}


Scene_GameOver::~Scene_GameOver() {
}

//called whene the scene is first loaded. Do any initial setup here
void Scene_GameOver::startScene() {
	gameSystem->removeAllGameObjects();
	gameSystem->addGameObjects("gameover_scene.txt");//loading instruction screen for now (for testing)
	gameSystem->levelLoaded = 3;
	gameSystem->markerPosition = 0;

	Msg* m = new Msg(LEVEL_LOADED, "3");
	msgBus->postMessage(m, gameSystem);

	PopulateTable(gameSystem->_gameOverList);
}

//called every frame of the gameloop
void Scene_GameOver::sceneUpdate() 
{

}

//called everytime a message is received by the gameSystem
void Scene_GameOver::sceneHandleMessage(Msg * msg) 
{
	// only one option; to go back to menu
	switch (msg->type)
	{
	case SPACEBAR_PRESSED:									// arbitrary relocate if mouse click fails
		gameSystem->loadScene(MAIN_MENU);
		break;
	case LEFT_MOUSE_BUTTON:									// When the left mouse is pushed
	{
		vector<string> objectData = split(msg->data, ',');
		INT32 x = atoi(objectData[0].c_str());
		INT32 y = atoi(objectData[1].c_str());
		INT32 width = atoi(objectData[2].c_str());
		INT32 length = atoi(objectData[3].c_str());
		x -= width / 2; y -= length / 2;	
		y = -y;												// Get location relative to center of screen
		bool change = false;


		for (GameObject *g : gameSystem->gameObjects)		// Search through the game objects
		{
			if ((x < g->x + (g->width / 2) && x > g->x - (g->width / 2)) &&	// Check bounds of items
				(y < g->y + (g->length / 2) && y > g->y - (g->length / 2)))
			{
				if (g->id.compare("GameOverMenuItem3") == 0)	// if it is the retreat button
				{
					// Load main menu
					msgBus->postMessage(new Msg(BUTTON_SELECT_SOUND), gameSystem);	// Post message to sound
					gameSystem->loadScene(MAIN_MENU);			// load Main Menu
					change = true;
					break;
				}
			}
		}
		if (change)												// Just so this is only called once
		{
			msgBus->postMessage(new Msg(LEVEL_LOADED, std::to_string(gameSystem->levelLoaded)), gameSystem);	// Post to change level
		}
		break;
	}
	case MOUSE_MOVE:												// When mouse is moved on game screen
	{
		vector<string> objectData = split(msg->data, ',');
		INT32 x = atoi(objectData[0].c_str());
		INT32 y = atoi(objectData[1].c_str());
		INT32 width = atoi(objectData[2].c_str());
		INT32 length = atoi(objectData[3].c_str());
		x -= width / 2; y -= length / 2;							// realtive position
		y = -y;
		bool change = false;
		auto it = gameSystem->gameObjects.begin();
		for (; it != gameSystem->gameObjects.end(); ++it)			// all game objects
		{
			if ((x < (*it)->x + ((*it)->width / 2) && x >(*it)->x - ((*it)->width / 2)) &&	// check bounds + location
				(y < (*it)->y + ((*it)->length / 2) && y >(*it)->y - ((*it)->length / 2)))
			{
				if ((*it)->id.compare("GameOverMenuItem3") == 0)	// if retreat button
				{
					gameSystem->markerPosition = 3; change = true;	// set marker and flag this has changed
					break;
				}
			}
		}
		if (it == gameSystem->gameObjects.end() && gameSystem->markerPosition == 3)	// If there was no object found + marker still is retreat button
		{
			change = true;
			gameSystem->markerPosition = 0;							// reset button
		}
		
		if (change) {												// Update retreat button with new render item
			if (gameSystem->markerPosition == 3)					
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverMenuItem" + to_string(3) + ",1,MenuItemSelected" + to_string(gameSystem->markerPosition) + ".png"), gameSystem);
			else
				msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverMenuItem" + to_string(3) + ",1,MenuItem" + to_string(3) + ".png"), gameSystem);
		}
		break;
	}
	}
}

/*
* Populates the table from a vector of strings from the game system
* Could only do this since scenes are deleted once exit
* goes through list and switches the renderable object with player name
*/
void Scene_GameOver::PopulateTable(const std::vector<std::string> & vec)
{
	int count = 4, which = 1;
	if (gameSystem->win)
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverHeader" + to_string(1) + ",1,Win" + ".png"), gameSystem);
	else
		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverHeader" + to_string(1) + ",1,Lose" + ".png"), gameSystem);

	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		if (it->compare("player1") == 0) which = 1;
		else if(it->compare("player2") == 0) which = 2;
		else if (it->compare("player3") == 0) which = 3;
		else if (it->compare("player4") == 0) which = 4;
		else if (it->compare("Here") == 0)
		{
			FullscreenObj * arrow = gameSystem->findFullscreenObject("GameOverArrow");
			if (count > 2)
				arrow->y = (((count - 2) / 2) * -50) - 25;
			if (count <= 2)
				arrow->y = (((count - 3) / 2) * -50) + 25;
			arrow->postPostionMsg();
			continue;
		}

		msgBus->postMessage(new Msg(UPDATE_OBJ_SPRITE, "GameOverPlace" + to_string(count) + ",1,Player" + to_string(which) + ".png"), gameSystem);
		count--;
	}
}