#pragma once

//#include "core.hpp"
#include <glm/glm.hpp>
#include <stdint.h>
#include <unordered_map>
#include <string>

#include "tracelog.hpp"
#include "core/coreapi.hpp"
#include "core/types.hpp"

struct Animation{
    uint16_t frames;
    glm::vec2 indices[60];  // Grid indices (row, col)
    glm::vec2 tileSize = {0, 0};  // Size of each tile in pixels
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
CORE_API void registryAnimation(const char* id, const uint16_t frames, const uint16_t row, const glm::vec2 tileSize, bool loop);
CORE_API Animation* getAnimation(const char* id);
CORE_API void destroyAnimationManager();

// Helper function to convert grid index to pixel Rect
CORE_API Rect gridToPixelRect(glm::vec2 gridIndex, glm::vec2 tileSize);
//}