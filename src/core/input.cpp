#include <string.h>

#include "input.hpp"
#include "tracelog.hpp"

Input* input;

void initInput(Arena* arena){
    //Input input = {};
    //input = new Input();
    input = arenaAllocStruct(arena, Input);
    input->fps = 0;
    memSet(input->keys, false, sizeof(input->keys));
    memSet(input->keysPrevFrame, false, sizeof(input->keysPrevFrame));
    memSet(input->gamepad.buttons, false, sizeof(input->gamepad.buttons));
    memSet(input->gamepad.buttonsPrevFrame, false, sizeof(input->gamepad.buttons));
    memSet(input->gamepad.trigger, false, sizeof(input->gamepad.trigger));
    memSet(input->mouseButtons, false, sizeof(input->mouseButtons));
    memSet(input->mouseButtonsPrevFrame, false, sizeof(input->mouseButtonsPrevFrame));
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

void updateInputState(float dt){
    memCopy(input->keysPrevFrame, input->keys, sizeof(input->keys)); //350 are the keys states watch input.hpp
    memCopy(input->gamepad.buttonsPrevFrame, input->gamepad.buttons, sizeof(input->gamepad.buttons));
    memCopy(input->mouseButtonsPrevFrame, input->mouseButtons, sizeof(input->mouseButtons));
    input->fps = 1.0f/dt;
}

float getFPS(){
    return input->fps;
}

Input* getInputState(){
    return input;
}