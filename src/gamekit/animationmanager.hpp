#pragma once

#include "core.hpp"
#include "export.hpp"

struct Animation{
    glm::vec2 indices[60];
    uint16_t frames;

    float frameDuration = 0;
    bool loop;
};

struct AnimationManager{
    std::unordered_map<std::string, Animation> animations;
};

//extern "C"{
AnimationManager initAnimationManager();
void registryAnimation(AnimationManager* manger, const char* id, const uint16_t frames, const uint16_t* indices, const uint32_t yOffset, bool loop);
void registryAnimation(AnimationManager* manger, const char* id, const uint16_t frames, const uint32_t yOffset, bool loop);
//KIT_API void registryAnimation(AnimationManager* manger, const char* id, const uint16_t frames, const glm::vec2* indices);
void registryAnimation(AnimationManager* manger, const char* id, const uint16_t frames, const glm::vec2* indices, const glm::vec2* offset, bool loop);
Animation* getAnimation(AnimationManager* manager, const char* id);
//}