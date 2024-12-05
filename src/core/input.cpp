#include <string.h>
#include <GLFW/glfw3.h>

#include "input.hpp"
#include "tracelog.hpp"

Input initInput(){
    Input input = {};
    memset(input.keys, false, sizeof(input.keys));
    memset(input.gamepad.buttons, false, sizeof(input.gamepad.buttons));
    memset(input.gamepad.trigger, false, sizeof(input.gamepad.trigger));
    return input;
}

void registerGamepadInput(Input* input){
    Gamepad& gamepad = input->gamepad;
    if(glfwJoystickPresent(gamepad.jid) && glfwJoystickIsGamepad(gamepad.jid)){
        GLFWgamepadstate state;
        if(glfwGetGamepadState(gamepad.jid, &state)){
            for(int button = 0; button < GLFW_GAMEPAD_BUTTON_LAST; button++){
                bool isPressed = state.buttons[button] == GLFW_PRESS;
                if(isPressed){
                    gamepad.buttons[button] = true;
                }else{
                    gamepad.buttons[button] = false;
                }
            }
        }
        gamepad.leftX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        gamepad.leftY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
        gamepad.rightX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
        gamepad.rightY = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
        (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] == true) ? gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER] = true : gamepad.trigger[GAMEPAD_AXIS_LEFT_TRIGGER] = false;
        (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] == true) ? gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER] = true : gamepad.trigger[GAMEPAD_AXIS_RIGHT_TRIGGER] = false;
    }
}