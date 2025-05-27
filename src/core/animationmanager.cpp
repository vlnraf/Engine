#include "animationmanager.hpp"

static AnimationManager* animationManager;

void initAnimationManager(){
    animationManager = new AnimationManager();
}

void registryAnimation(const char* id, const uint16_t frames, const uint16_t* indices, const uint32_t yOffset, bool loop){
    Animation anim = {};
    anim.frames = frames;
    for(int i = 0; i < frames; i++){
        anim.indices[i] = {indices[i], yOffset};
    }
    anim.frameDuration = 1.0f / frames;
    anim.loop = loop;

    animationManager->animations.insert({id, anim});
}

void registryAnimation(const char* id, const uint16_t frames, const uint32_t yOffset, bool loop){
    Animation anim = {};
    anim.frames = frames;
    for(int i = 0; i < frames; i++){
        anim.indices[i] = {i, yOffset};
    }
    anim.frameDuration = 1.0f / frames;
    anim.loop = loop;

    animationManager->animations.insert({id, anim});
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