#include "core.hpp"
#include "mainmenu.hpp"
#include "projectx.hpp"

void advanceMenu(MenuState* state, int advance){
    state->focusItem += advance;
    if(state->focusItem < 0) state->focusItem += MenuItems::MENU_TOTAL;
    state->focusItem = state->focusItem % MenuItems::MENU_TOTAL;
}

void enterMenu(MenuState* state){
    if(state->focusItem == MenuItems::MENU_PLAY){
        loadLevel(GameLevels::FIRST_LEVEL);
        gameState->gameLevels = GameLevels::FIRST_LEVEL;
    }
}

void drawButton(MenuState* state, const char* text, Font* font, float fontScale, glm::vec4 rect, MenuItems index){
    glm::vec2 canvasSize = {getScreenSize().x, getScreenSize().y};
    glm::vec2 buttonSize = {rect.z, rect.w};
    glm::vec2 pos = {rect.x - (buttonSize.x / 2), rect.y};
    glm::vec4 color = {0,0,0,0.5f};
    glm::vec4 focusColor = {1,0,0,0.5f};
    glm::vec4 currentColor = color;
    if(state->focusItem == index){
        currentColor = focusColor;
    }
    renderDrawFilledRectPro(pos, buttonSize, 0, {0,1}, currentColor, 0.0f);
    uint32_t fontHeight= round((font->characterSize * fontScale));
    uint32_t fontWidth = calculateTextWidth(font, text, fontScale);
    glm::vec2 fontPos = {pos.x + (buttonSize.x / 2) - (fontWidth / 2), pos.y - (buttonSize.y) + (fontHeight / 2)};
    renderDrawText2D(font, text, fontPos, fontScale);
}

void handleMenuInput(){
    MenuState* state = &gameState->menuState;
    if(isJustPressed(KEYS::Down)){
        advanceMenu(state, 1);
    }else if(isJustPressed(KEYS::Up)){
        advanceMenu(state, -1);
    }else if(isJustPressed(KEYS::Enter)){
        enterMenu(state);
    }
}

void drawMenu(){
    glm::vec2 canvasSize = {getScreenSize().x, getScreenSize().y};
    //int yo = engine->mainCamera.height / 2;
    //int xo = engine->mainCamera.width / 2;
    int xo = getScreenSize().x / 2;
    int yo = getScreenSize().y / 2;
    int padding = 10;
    Font* font = getFont("Roboto-Regular");
    glm::vec2 buttonSize = {getScreenSize().x / 2, getScreenSize().y / 10};
    //float fontScale = (getScreenSize().y / getScreenSize().y);
    //float fontScale = buttonSize.y / font->maxHeight;
    float fontScale = 1.0f;


    Texture* controllerTexture = getTextureByName("XOne");
    //renderDrawQuad2D(controllerTexture, convertScreenCoords({xo - (controllerTexture->size.x / 2), yo - controllerTexture->size.y}, controllerTexture->size, canvasSize), {1,1}, {0,0}, {0,0}, controllerTexture->size);
    renderDrawQuad2D(convertScreenCoords({xo - (controllerTexture->size.x / 2), yo - controllerTexture->size.y}, controllerTexture->size, canvasSize), controllerTexture->size, 0, controllerTexture);

    drawButton(&gameState->menuState, "Play", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_PLAY);
    yo -= buttonSize.y + padding;
    drawButton(&gameState->menuState, "Load save", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_LOAD);
    yo -= buttonSize.y + padding;
    drawButton(&gameState->menuState, "Options", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_OPTIONS);
    yo -= buttonSize.y + padding;
    drawButton(&gameState->menuState, "Quit", font, fontScale, {xo, yo, buttonSize.x, buttonSize.y}, MenuItems::MENU_QUIT);

    //Texture* controllerTexture = getTexture("Xone");
    //renderDrawQuad2D(controllerTexture, {0,0},{1,1},{0,0},{0,0}, controllerTexture->size);
    //renderDrawFilledRect({100,100}, {100, 100}, 0, {1,0,0,1});
    //renderDrawRect({0,0},{100,100},{1,0,0,1}, 1);
}
