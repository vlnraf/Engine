#include <string.h>

#include "input.hpp"
#include "tracelog.hpp"

static Input* input;

void initInput(Arena* arena){
    //Input input = {};
    //input = new Input();
    input = arenaAllocStruct(arena, Input);
    memset(input->keys, false, sizeof(input->keys));
    memset(input->keysPrevFrame, false, sizeof(input->keysPrevFrame));
    memset(input->gamepad.buttons, false, sizeof(input->gamepad.buttons));
    memset(input->gamepad.buttonsPrevFrame, false, sizeof(input->gamepad.buttons));
    memset(input->gamepad.trigger, false, sizeof(input->gamepad.trigger));
    memset(input->mouseButtons, false, sizeof(input->mouseButtons));
    memset(input->mouseButtonsPrevFrame, false, sizeof(input->mouseButtonsPrevFrame));
}

bool isPressed(int key){
    return input->keys[key];
}

bool wasPressed(int key){
    return input->keysPrevFrame[key];
}

bool isJustPressed(int key){
    return input->keys[key] && !input->keysPrevFrame[key];
}

bool wasPressedGamepad(int key){
    return input->gamepad.buttonsPrevFrame[key];
}
bool isPressedGamepad(int key){
    return input->gamepad.buttons[key];
}
bool isJustPressedGamepad(int key){
    return input->gamepad.buttons[key] && !input->gamepad.buttonsPrevFrame[key];
}

glm::vec2 getMousePos(){
    return input->mousePos;
}

bool isMouseButtonPressed(int button){
    return input->mouseButtons[button];
}

bool isMouseButtonJustPressed(int button){
    return input->mouseButtons[button] && !input->mouseButtonsPrevFrame[button];
}

bool isMouseButtonRelease(int button){
    return !input->mouseButtons[button] && input->mouseButtonsPrevFrame[button];
}

void updateInputState(){
    memcpy(input->keysPrevFrame, input->keys, sizeof(input->keys)); //350 are the keys states watch input.hpp
    memcpy(input->gamepad.buttonsPrevFrame, input->gamepad.buttons, sizeof(input->gamepad.buttons));
    memcpy(input->mouseButtonsPrevFrame, input->mouseButtons, sizeof(input->mouseButtons));
}

Input* getInputState(){
    return input;
}