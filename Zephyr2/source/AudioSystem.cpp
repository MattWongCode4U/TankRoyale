#include "AudioSystem.h"

AudioSystem::AudioSystem(MessageBus* mbus) : System(mbus)
{
}

AudioSystem::~AudioSystem()
{
}

void AudioSystem::startSystemLoop()
{
	clock_t thisTime = clock();

	int currentGameTime = 0;
	while (alive) {
		thisTime = clock();
		if (thisTime  < currentGameTime) {
			Sleep(currentGameTime - thisTime);
		}

		Audio.Update();
		currentGameTime = thisTime + timeFrame;
		handleMsgQ();
	}
}

void AudioSystem::handleMessage(Msg *msg)
{
	System::handleMessage(msg);
	vector<std::string> data;

	switch (msg->type) {
	case CAMERA_OFFSET:
		data = split(msg->data, ',');
		listenerX = stof(data[0]) * -1;
		listenerY = stof(data[1]);
		UpdateListener(listenerX, listenerY);
		/*
		OutputDebugString("CAMERA OFFSET ");
		OutputDebugString(to_string(listenerX).c_str());
		OutputDebugString("  ");
		OutputDebugString(to_string(listenerY).c_str());
		OutputDebugString("\n");
		*/
		break;
	case GO_ADDED:
	{
		data = split(msg->data, ',');
		xPos = stof(data[2]);
		yPos = stof(data[3]);
		if (data[9] == "Cannonball")
		{
			if (yPos == listenerY)
				sfxVolume = 5;
			else
				sfxVolume = 40;
			PlaySfx(CANNONBALL_SFX, true, xPos, yPos, sfxVolume);
			/*
			OutputDebugString("cannonball sfx\n");
			OutputDebugString(to_string(xPos).c_str());
			OutputDebugString("  ");
			OutputDebugString(to_string(yPos).c_str());
			OutputDebugString("\n");
			*/
		}
		break;
	}
	case CHANGE_MAST:
		data = split(msg->data, ',');
		if (data[0] == "playerShip")
		{
			PlaySfx(SAIL_SFX, false, 0, 0, 10);
		}
		break;
	case CHANGE_RUDDER:
		data = split(msg->data, ',');
		if (data[0] == "playerShip")
		{
			PlaySfx(RUDDER_SFX, false, 0, 0, 10);
		}
		break;
	case SHIP_SANK:
		data = split(msg->data, ',');
		xPos = stof(data[1]);
		yPos = stof(data[2]);
		if (yPos == listenerY)
			sfxVolume = 5;
		else
			sfxVolume = 40;
		PlaySfx(DESTROYED_SFX, true, xPos, yPos, sfxVolume);
		break;
	case LEVEL_LOADED:
		loadedLevel = atoi(msg->data.c_str());
		if (loadedLevel == 2)
		{
			if (loadedLevel != lastLevel)
			{
				PlayMusic(SOUNDTRACK_GAMEPLAY, GAMEPLAY_VOLUME);
				StopMusic(SOUNDTRACK_MENU);
			}
		}
		else
		{
			if (lastLevel == 2)
			{
				PlayMusic(SOUNDTRACK_MENU, DEFAULT_VOLUME);
				StopMusic(SOUNDTRACK_GAMEPLAY);
			}
		}

		break;
	case AUDIO_MUTE:
		audioMute = atoi(msg->data.c_str());
		if (audioMute == 1)
			MuteAudio(true);
		else
			MuteAudio(false);
		break;
	default:
		break;
	}
	lastLevel = loadedLevel;
}

void AudioSystem::PlayMusic(string songName, float volume)
{
	Audio.LoadSound(songName, false, true, false);
	Audio.PlayAudio(songName, Vector3{ 0, 0, 0 }, volume);
}

void AudioSystem::StopMusic(string songName)
{
	Audio.UnLoadSound(songName);
}

void AudioSystem::PlaySfx(string soundName, bool directional, float x, float y, float volume)
{
	Audio.LoadSound(soundName, directional, false, false);
	Audio.PlayAudio(soundName, Vector3{ x, y, 0 }, volume);
}

void AudioSystem::StopSfx(string soundName)
{
	Audio.UnLoadSound(soundName);
}

void AudioSystem::MuteAudio(bool mute)
{
	Audio.Mute(mute);
}

void AudioSystem::StopAllAudio()
{
	Audio.StopAll();
}

void AudioSystem::UpdateListener(float x, float y)
{
	Audio.SetListenerLocation(Vector3{ x, y, 0 });
}