#include "core.hpp"
#include "tracelog.hpp"

// Web audio stub - no FMOD for web build

bool initAudioEngine(){
    LOGINFO("Audio disabled for web build (will add Web Audio API later)");
    return true;
}

void updateAudio(){
    // No-op for web
}

void loadAudio(const char* filename, bool loop){
    // No-op for web
}

void playAudio(const char* filename, float volume){
    // No-op for web
}

void setAudioVolume(const char* filename, float volume){
    // No-op for web
}

void stopAudio(const char* filename){
    // No-op for web
}

void destroyAudioEngine(){
    // No-op for web
}
