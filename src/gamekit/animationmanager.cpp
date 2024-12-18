#include "animationmanager.hpp"

KIT_API AnimationManager initAnimationManager(){
    AnimationManager animationManager = {};

    return animationManager;
}

KIT_API void registryAnimation(AnimationManager* manger, const char* id, const uint16_t frames, const uint16_t* indices, const uint32_t yOffset){
    Animation anim = {};
    anim.frames = frames;
    for(int i = 0; i < frames; i++){
        anim.indices[i] = {indices[i], yOffset};
    }
    anim.frameDuration = 1.0f / frames;

    manger->animations.insert({id, anim});
}

KIT_API Animation* getAnimation(AnimationManager* manager, const char* id){
    auto anim = manager->animations.find(id);
    if(anim != manager->animations.end()){
        return &manager->animations.at(id);
    }
    LOGERROR("Animation %s does not exist", id);
    return nullptr;
}