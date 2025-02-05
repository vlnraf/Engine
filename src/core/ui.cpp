#include "ui.hpp"

void button(const char* name, const glm::vec2 pos, const glm::vec2 size){
    pushButton(pos, size);
    pushText(name, pos, 1.0f);
}