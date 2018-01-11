#include "AudioEngine.h"

//AUDIO MANAGER SECTION
AudioManager::AudioManager()
{
	mpSystem = NULL;
	AudioEngine::FmodError(FMOD::System_Create(&mpSystem));
	AudioEngine::FmodError(mpSystem->init(512, FMOD_INIT_NORMAL, 0));
}

void AudioManager::Update()
{
	
	vector<ChannelMap::iterator> finishedChannels;
	for (auto it = ChannelObjects.begin(), itEnd = ChannelObjects.end(); it != itEnd; ++it)
	{
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
		{
			finishedChannels.push_back(it);
		}
	}
	for (auto& it : finishedChannels)
	{
		ChannelObjects.erase(it);
		
	}
	AudioEngine::FmodError(mpSystem->update());
}

void AudioManager::Mute()
{
	for (auto it = ChannelObjects.begin(), itEnd = ChannelObjects.end(); it != itEnd; ++it)
	{
		it->second->setMute(true);
	}
}

void AudioManager::UnMute()
{
	for (auto it = ChannelObjects.begin(), itEnd = ChannelObjects.end(); it != itEnd; ++it)
	{
		it->second->setMute(false);
	}
}

AudioManager* audioManager = nullptr;

//AUDIO ENGINE SECTION
AudioEngine::AudioEngine()
{
	audioManager = new AudioManager;
	muteAudio = false;
}

AudioEngine::~AudioEngine()
{
	delete audioManager;
}

void AudioEngine::Update()
{
	
	audioManager->Update();
}
//TEST
int AudioEngine::FmodError(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		//cout << "FMOD ERROR " << result << endl;
		return 1;
	}
	// cout << "FMOD all good" << endl;
	return 0;
}

void AudioEngine::LoadSound(const string &strSoundName, bool b3d, bool bLooping, bool bStream)
{
	auto tFoundIt = audioManager->SoundObjects.find(strSoundName);
	if (tFoundIt != audioManager->SoundObjects.end())
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;
	AudioEngine::FmodError(audioManager->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
	if (pSound) {
		audioManager->SoundObjects[strSoundName] = pSound;
	}

}
void AudioEngine::UnLoadSound(const string &strSoundName)
{
	auto tFoundIt = audioManager->SoundObjects.find(strSoundName);
	if (tFoundIt == audioManager->SoundObjects.end())
		return;

	AudioEngine::FmodError(tFoundIt->second->release());
	audioManager->SoundObjects.erase(tFoundIt);
}
void AudioEngine::SetListenerLocation(const Vector3 &vPosition)
{
	FMOD_VECTOR fmvPos;
	fmvPos = VectorToFmod(vPosition);
	audioManager->mpSystem->set3DListenerAttributes(0, &fmvPos, 0, 0, 0);
}
int AudioEngine::PlayAudio(const string &strSoundName, const Vector3 &vPosition, float fVolumedB)
{
	int nChannelId = audioManager->nextChannelId++;
	auto tFoundIt = audioManager->SoundObjects.find(strSoundName);
	if (tFoundIt == audioManager->SoundObjects.end())
	{
		LoadSound(strSoundName, true, false, false);
		tFoundIt = audioManager->SoundObjects.find(strSoundName);
		if (tFoundIt == audioManager->SoundObjects.end())
		{
			return nChannelId;
		}
	}

	FMOD::Channel* pChannel = nullptr;

	//for (auto it = audioManager->ChannelObjects.begin(), itEnd = audioManager->ChannelObjects.end(); it != itEnd; ++it)
	//{
	//	bool bIsPlaying = false;
	//	it->second->isPlaying(&bIsPlaying);
	//	if (!bIsPlaying)
	//	{
	//		pChannel = it->second;
	//	}
	//}


	
	AudioEngine::FmodError(audioManager->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
	if (pChannel)
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D) {
			FMOD_VECTOR position = VectorToFmod(vPosition);
			AudioEngine::FmodError(pChannel->set3DAttributes(&position, nullptr));
		}
		AudioEngine::FmodError(pChannel->setVolume(dbToVolume(fVolumedB)));
		AudioEngine::FmodError(pChannel->setPaused(false));
		pChannel->setMute(muteAudio);
		audioManager->ChannelObjects[nChannelId] = pChannel;
	}
	return nChannelId;
}

void AudioEngine::SetChannel3dPosition(int nChannelId, const Vector3 &vPosition)
{
	auto tFoundIt = audioManager->ChannelObjects.find(nChannelId);
	if (tFoundIt == audioManager->ChannelObjects.end())
		return;

	FMOD_VECTOR position = VectorToFmod(vPosition);
	AudioEngine::FmodError(tFoundIt->second->set3DAttributes(&position, NULL));
}
void AudioEngine::SetChannelvolume(int nChannelId, float fVolumedB)
{
	auto tFoundIt = audioManager->ChannelObjects.find(nChannelId);
	if (tFoundIt == audioManager->ChannelObjects.end())
		return;

	AudioEngine::FmodError(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}
float AudioEngine::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}
float AudioEngine::VolumeTodb(float volume)
{
	return 20.0f * log10f(volume);
}
FMOD_VECTOR AudioEngine::VectorToFmod(const Vector3 &vPosition)
{
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

void AudioEngine::Mute(bool mute)
{
	if (mute)
		audioManager->Mute();
	else
		audioManager->UnMute();

	muteAudio = mute;
}

void AudioEngine::StopAll()
{

}