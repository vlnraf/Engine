#if defined(_WIN32)
    #include <fmod.hpp>
    #include <fmod_errors.h>
#endif

#include <unordered_map>
#include <string>

#include "audioengine.hpp"
#include "tracelog.hpp"

#if defined(_WIN32)
struct AudioEngine{
    FMOD::System* system;
    std::unordered_map<std::string, FMOD::Sound*> soundManager;
    std::unordered_map<std::string, FMOD::Channel*> channelManager;
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

void loadAudio(const char* filename, bool loop){
    if(audioEngine->soundManager.find(filename) == audioEngine->soundManager.end()){
        FMOD::Sound* sound = nullptr;
        if(loop){
            audioEngine->system->createSound(filename, FMOD_LOOP_NORMAL, nullptr, &sound);
        }else{
            audioEngine->system->createSound(filename, FMOD_DEFAULT, nullptr, &sound);
        }
        audioEngine->soundManager.insert({filename, sound});
    }else{
        LOGERROR("Collision in sound loading occurred, this sound would not be loaded");
    }
}

void playAudio(const char* filename, float volume){
    if(audioEngine->soundManager.find(filename) == audioEngine->soundManager.end()){
        //loadAudio(filename);
        LOGERROR("No audio find");
        return;
    }
    FMOD::Sound* sound = audioEngine->soundManager.at(filename);
    FMOD::Channel* channel = nullptr;
    audioEngine->system->playSound(sound, nullptr, false, &channel);
    channel->setVolume(volume);
    if(audioEngine->channelManager.find(filename) == audioEngine->channelManager.end()){
        audioEngine->channelManager[filename] = channel;
    }
}

void setAudioVolume(const char* filename, float volume) {
    if (audioEngine->channelManager.find(filename) != audioEngine->channelManager.end()) {
        FMOD::Channel* channel = audioEngine->channelManager.at(filename);
        channel->setVolume(volume);
    } else {
        LOGERROR("Channel not found for %s", filename);
    }
}

void updateAudio(){
    audioEngine->system->update();
}

void destroyAudioEngine(){
    delete audioEngine;
}
#else
struct AudioEngine{};

static AudioEngine* audioEngine;

bool initAudioEngine(){return true;}

void loadAudio(const char* filename){}

void playAudio(const char* filename){}

void updateAudio(){}

void destroyAudioEngine(){}
#endif