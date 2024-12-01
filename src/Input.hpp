#pragma once

#include "keys.hpp"

struct Input{
    bool keys[256];
};

Input* initInput();