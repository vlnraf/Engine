#include <fmod.hpp>
#include <fmod_errors.h>
#include <unordered_map>
#include <string>

#include "audioengine.hpp"
#include "tracelog.hpp"

struct AudioEngine{
    FMOD::System* system;
    std::unordered_map<std::string, FMOD::Sound*> soundManager;
};

static AudioEngine* audioEngine;

bool initAudioEngine(){
    audioEngine = new AudioEngine();
    FMOD_RESULT result;
    //audioEngine->system = nullptr;

    result = FMOD::System_Create(&audioEngine->system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        LOGERROR("FMOD error! (%d) %s", result, FMOD_ErrorString(result));
        return false;
    }

    result = audioEngine->system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        LOGERROR("FMOD error! (%d) %s", result, FMOD_ErrorString(result));
        return false;
    }

    return true;
}

void loadAudio(const char* filename){
    FMOD::Sound* sound = nullptr;
    audioEngine->system->createSound(filename, FMOD_DEFAULT, nullptr, &sound);
    audioEngine->soundManager.insert({filename, sound});
}

void playAudio(const char* filename){
    if(audioEngine->soundManager.find(filename) == audioEngine->soundManager.end()){
        loadAudio(filename);
    }
    FMOD::Sound* sound = audioEngine->soundManager.at(filename);
    FMOD::Channel* channel = nullptr;
    audioEngine->system->playSound(sound, nullptr, false, &channel);
}

void updateAudio(){
    audioEngine->system->update();
}

void destroyAudioEngine(){
    delete audioEngine;
}