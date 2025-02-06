#include <string.h>

#include "input.hpp"
#include "tracelog.hpp"

Input* initInput(){
    //Input input = {};
    Input* input = new Input();
    memset(input->keys, false, sizeof(input->keys));
    memset(input->keysPrevFrame, false, sizeof(input->keysPrevFrame));
    memset(input->gamepad.buttons, false, sizeof(input->gamepad.buttons));
    memset(input->gamepad.buttonsPrevFrame, false, sizeof(input->gamepad.buttons));
    memset(input->gamepad.trigger, false, sizeof(input->gamepad.trigger));
    return input;
}

void destroyInput(Input* input){
    delete input;
}

bool isPressed(Input* input, int key){
    return input->keys[key];
}

bool wasPressed(Input* input, int key){
    return input->keysPrevFrame[key];
}

bool isJustPressed(Input* input, int key){
    return input->keys[key] && !input->keysPrevFrame[key];
}

bool wasPressedGamepad(Gamepad* gamepad, int key){
    return gamepad->buttonsPrevFrame[key];
}
bool isPressedGamepad(Gamepad* gamepad, int key){
    return gamepad->buttons[key];
}
bool isJustPressedGamepad(Gamepad* gamepad, int key){
    return gamepad->buttons[key] && !gamepad->buttonsPrevFrame[key];
}
