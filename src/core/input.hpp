#pragma once

#include <glm/glm.hpp>

#include "core/coreapi.hpp"
#include "keys.hpp"

#define GAMEPAD_BUTTON_A               0
#define GAMEPAD_BUTTON_B               1
#define GAMEPAD_BUTTON_X               2
#define GAMEPAD_BUTTON_Y               3
#define GAMEPAD_BUTTON_LEFT_BUMPER     4
#define GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define GAMEPAD_BUTTON_BACK            6
#define GAMEPAD_BUTTON_START           7
#define GAMEPAD_BUTTON_GUIDE           8
#define GAMEPAD_BUTTON_LEFT_THUMB      9
#define GAMEPAD_BUTTON_RIGHT_THUMB     10
#define GAMEPAD_BUTTON_DPAD_UP         11
#define GAMEPAD_BUTTON_DPAD_RIGHT      12
#define GAMEPAD_BUTTON_DPAD_DOWN       13
#define GAMEPAD_BUTTON_DPAD_LEFT       14
#define GAMEPAD_BUTTON_LAST            GAMEPAD_BUTTON_DPAD_LEFT

#define GAMEPAD_BUTTON_CROSS       GAMEPAD_BUTTON_A
#define GAMEPAD_BUTTON_CIRCLE      GAMEPAD_BUTTON_B
#define GAMEPAD_BUTTON_SQUARE      GAMEPAD_BUTTON_X
#define GAMEPAD_BUTTON_TRIANGLE    GAMEPAD_BUTTON_Y

#define GAMEPAD_AXIS_LEFT_X        0
#define GAMEPAD_AXIS_LEFT_Y        1
#define GAMEPAD_AXIS_RIGHT_X       2
#define GAMEPAD_AXIS_RIGHT_Y       3
#define GAMEPAD_AXIS_LEFT_TRIGGER  4
#define GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define GAMEPAD_AXIS_LAST          GAMEPAD_AXIS_RIGHT_TRIGGER

struct Gamepad{
    int jid;
    const char* name;
    bool buttons[15];
    bool buttonsPrevFrame[15];
    float leftX, leftY;
    float rightX, rightY;
    bool trigger[2];
};

struct Input{
    bool keys[350];
    bool keysPrevFrame[350];
    glm::vec2 mousePos;
    //float mousePosX;
    //float mousePosY;
    Gamepad gamepad;
};

CORE_API void initInput();
CORE_API void destroyInput();
//void registerGamepadInput(Input* input);
CORE_API bool isPressed(int key);
CORE_API bool wasPressed(int key);
CORE_API bool isJustPressed(int key);
CORE_API bool isJustPressedGamepad(int key);
CORE_API bool isPressedGamepad(int key);
CORE_API bool wasPressedGamepad(int key);
CORE_API glm::vec2 getMousePos();
CORE_API void updateInputState();
CORE_API Input* getInputState();