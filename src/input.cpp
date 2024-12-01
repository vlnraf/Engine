#include <string.h>

#include "input.hpp"

Input* initInput(){
    Input* input = new Input();
    memset(input->keys, false, sizeof(input->keys));
    return input;
}