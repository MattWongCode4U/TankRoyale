#pragma once
#include "AudioEngine.h"
#include "System.h"

#define DEFAULT_VOLUME 5
#define GAMEPLAY_VOLUME 5
#define SOUNDTRACK_MENU "Assets/Music/MenuSong.mp3"
#define SOUNDTRACK_GAMEPLAY "Assets/Music/QuarrellingSeas.mp3"
#define CANNONBALL_SFX "Assets/Sfx/CannonballSfx.mp3"
#define SAIL_SFX "Assets/Sfx/SailSfx.mp3"
#define RUDDER_SFX "Assets/Sfx/RudderSfx.mp3"
#define DESTROYED_SFX "Assets/Sfx/DestroyedSfx.mp3"
#define REGULAR_SHOT_SFX "Assets/Sfx/LaserShot.mp3"
#define SNIPER_SHOT_SFX "Assets/Sfx/SniperShot.mp3"
#define ARTILLERY_SHOT_SFX "Assets/Sfx/Explosion.mp3"
#define MOVEMENT_SFX "Assets/Sfx/HoverSound.mp3"
#define BUTTON_SELECT_SFX "Assets/Sfx/GUI_Select.mp3"
#define GAMEPLAY_SND "Assets/Sfx/IGT.mp3"
#define MENU_SND "Assets/Sfx/LastDawn.mp3"

class AudioSystem : public System
{
public:
	AudioEngine Audio;
	string currentBGM;
	int loadedLevel;
	int lastLevel = 2;
	int audioMute;
	int gameplayVolume = GAMEPLAY_VOLUME;
	float listenerX = 0;
	float listenerY = 0;
	float xPos;
	float yPos;
	float sfxVolume = 5;

	std::ostringstream ss;

	AudioSystem(MessageBus* mbus);
	~AudioSystem();

	void startSystemLoop();
	void handleMessage(Msg *msg);

	void PlayMusic(string songName, float volume);
	void StopMusic(string songName);
	void PlaySfx(string soundName, float volume);
	void PlaySfx(string soundName, bool directional, float x, float y, float volume);
	void StopSfx(string soundName);
	void MuteAudio(bool mute);
	void StopAllAudio();
	void UpdateListener(float x, float y);

	int timeFrame = 20;
};

