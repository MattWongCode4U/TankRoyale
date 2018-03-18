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
			std::this_thread::sleep_for(std::chrono::milliseconds(currentGameTime - thisTime));
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
	// Tank shots
	case REGULAR_SHOT_SOUND:
		PlaySfx(REGULAR_SHOT_SFX, gameplayVolume);
		break;
	case SNIPER_SHOT_SOUND:
		PlaySfx(SNIPER_SHOT_SFX, gameplayVolume);
		break;
	case ARTILLERY_SHOT_SOUND:
		PlaySfx(ARTILLERY_SHOT_SFX, gameplayVolume);
		break;
	// Tank Move
	case MOVEMENT_SOUND:
		PlaySfx(MOVEMENT_SFX, gameplayVolume);
		break;
	// Button Selection
	case BUTTON_SELECT_SOUND:
		PlaySfx(BUTTON_SELECT_SFX, gameplayVolume);
		break;
	case AUDIO_SET:
		gameplayVolume = stoi(msg->data);
		Audio.SetSoundVolume(gameplayVolume);
		break;
	case AUDIO_DOWN:
		if (gameplayVolume > 0) gameplayVolume--;
		else if (gameplayVolume < 0) gameplayVolume = 0;
		Audio.SetSoundVolume(gameplayVolume);
		break;
	case AUDIO_UP:
		if (gameplayVolume < 0) gameplayVolume++;
		else if (gameplayVolume > GAMEPLAY_VOLUME) gameplayVolume = GAMEPLAY_VOLUME;
		Audio.SetSoundVolume(gameplayVolume);
		break;
	case AUDIO_MUTE:
		audioMute = atoi(msg->data.c_str());
		if (audioMute == 1)
			MuteAudio(true);
		else
			MuteAudio(false);
		break;
	// Level Sounds
	case LEVEL_LOADED:
		loadedLevel = atoi(msg->data.c_str());
		if (loadedLevel == 2)
		{
			if (loadedLevel != lastLevel)
			{
				PlayMusic(GAMEPLAY_SND, gameplayVolume);
				StopMusic(MENU_SND);
			}
		}
		else
		{
			if (lastLevel == 2)
			{
				PlayMusic(MENU_SND, gameplayVolume);
				StopMusic(GAMEPLAY_SND);
			}
		}
		break;
	case MAIN_MENU_SOUND:
		break;
	case GAME_MENU_SOUND:
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

void AudioSystem::PlaySfx(string soundName, float volume)
{
	Audio.LoadSound(soundName, false, false, false);
	Audio.PlayAudio(soundName, Vector3{ 0,0,0 }, volume);
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