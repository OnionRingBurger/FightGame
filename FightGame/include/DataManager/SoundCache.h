#pragma once

#include <string>
#include <unordered_map>
//#include <vector>

#include "Sound.h"

class SoundCache
{
public:
	SoundCache();
	~SoundCache();

	void LoadSound(std::string);
	

private:
	std::unordered_map<std::string, XAUDIO2_BUFFER*> soundCache;

};