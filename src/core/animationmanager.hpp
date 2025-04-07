#pragma once

//#include "core.hpp"
#include <glm/glm.hpp>
#include <stdint.h>
#include <unordered_map>
#include <string>

#include "tracelog.hpp"
#include "core/coreapi.hpp"

struct Animation{
    uint16_t frames;
    glm::vec2 indices[60];
    //int tileIds[60];

    float frameDuration = 0;
    float elapsedTime = 0;
    int currentFrame = 0;
    bool loop;
};

struct AnimationManager{
    std::unordered_map<std::string, Animation> animations;
};

//extern "C"{
CORE_API void initAnimationManager();
CORE_API void registryAnimation(const char* id, const uint16_t frames, const uint16_t* indices, const uint32_t yOffset, bool loop);
CORE_API void registryAnimation(const char* id, const uint16_t frames, const uint32_t yOffset, bool loop);
//KIT_API void registryAnimation(AnimationManager* manger, const char* id, const uint16_t frames, const glm::vec2* indices);
CORE_API void registryAnimation(const char* id, const uint16_t frames, const glm::vec2* indices, const glm::vec2* offset, bool loop);
CORE_API Animation* getAnimation(const char* id);
CORE_API void destroyAnimationManager();
//}