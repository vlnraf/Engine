#include "animationmanager.hpp"

AnimationManager* animationManager;

void initAnimationManager(){
    animationManager = new AnimationManager();
}

void registryAnimation(const char* id, const uint16_t frames, const uint16_t row, const glm::vec2 tileSize, bool loop){
    Animation anim = {};
    anim.frames = frames;
    anim.tileSize = tileSize;
    for(int i = 0; i < frames; i++){
        anim.indices[i] = {(float)i, (float)row};  // Sequential frames in specified row
    }
    anim.frameDuration = 1.0f / frames;
    anim.loop = loop;

    animationManager->animations.insert({id, anim});
}

// Helper function to convert grid index to pixel Rect
Rect gridToPixelRect(glm::vec2 gridIndex, glm::vec2 tileSize){
    return Rect{
        .pos = {gridIndex.x * tileSize.x, gridIndex.y * tileSize.y},
        .size = tileSize
    };
}

Animation* getAnimation(const char* id){
    auto anim = animationManager->animations.find(id);
    if(anim != animationManager->animations.end()){
        return &animationManager->animations.at(id);
    }
    LOGERROR("Animation %s does not exist", id);
    return nullptr;
}

void destroyAnimationManager(){
    delete animationManager;
}
