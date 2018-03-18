#pragma once
#include <windows.h>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <time.h>
#include <thread>
#include "fmod.hpp"
#include "fmod_errors.h"
#include "GlobalPrefs.h"

using namespace std;

struct Vector3 {
	float x;
	float y;
	float z;
};

struct AudioManager {
	AudioManager();
	~AudioManager();

	void Update();
	void Mute();
	void UnMute();

	FMOD::System* mpSystem;

	int nextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;

	SoundMap SoundObjects;
	ChannelMap ChannelObjects;
};

class AudioEngine
{
public:
	bool muteAudio;

	AudioEngine();
	~AudioEngine();

	static void Update();
	static int FmodError(FMOD_RESULT result);

	void LoadSound(const string &strSoundName, bool b3d, bool bLooping, bool bStream);
	void UnLoadSound(const string &strSoundName);
	void SetListenerLocation(const Vector3 &vPosition);
	int PlayAudio(const string &strSoundName, const Vector3 &vPosition = Vector3{ 0, 0, 0 }, float fVolumedB = 5.0f);
	void SetChannel3dPosition(int nChannelId, const Vector3 &vPosition);
	void SetChannelvolume(int nChannelId, float fVolumedB);
	void SetSoundVolume(int volume);
	float dbToVolume(float db);
	float VolumeTodb(float volume);
	FMOD_VECTOR VectorToFmod(const Vector3 &vPosition);
	void Mute(bool mute);
	void StopAll();
};
