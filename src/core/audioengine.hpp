#pragma once

#include "core/coreapi.hpp"
#include "coreapi.hpp"

CORE_API bool initAudioEngine();
CORE_API void loadAudio(const char* filename);
CORE_API void playAudio(const char* filename);
CORE_API void updateAudio();
CORE_API void destroyAudioEngine();