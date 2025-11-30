#include "ui.hpp"
#include "input.hpp"
#include "renderer/renderer.hpp"

#include "core.hpp"

static UIState* uiState;

UIState* initUI(Arena* arena){
    //uiState = new UIState();
    uiState = arenaAllocStruct(arena, UIState);
    uiState->id = 1;
    uiState->hot = 0;
    uiState->active = 0;
    uiState->font = getFont("Minecraft");
    return uiState;
}

void setFontUI(Font* font){
    uiState->font = font;
}

Font* getFontUI(){
    return uiState->font;
}

void destroyUI(){
    //delete uiState;
}

std::vector<std::string> splitText(const char* text, const char separator){
    std::vector<std::string> result;
    //char word[100];
    std::string word;
    int j = 0;
    for(int i = 0; text[i] != '\0'; i++){
        if(text[i] != separator){
            word[j] = text[i];
            j++;
        }else{
            word[j] = '\0';
            result.push_back(word);
            //memset(word, 0, sizeof(char)*100);
            //i++;
            j=0;
        }
    }
    return result;
}

void beginUiFrame(glm::vec2 canvasPos, glm::vec2 canvasSize){
    uiState->canvasSize = canvasSize;
    uiState->mousePos = getMousePos();
    glm::vec2 scale = uiState->canvasSize / getScreenSize();
    uiState->mousePos = uiState->mousePos * scale;

    //beginUIRender(canvasPos, canvasSize);
    //OrtographicCamera uiCamera = createCamera({canvasPos.x,canvasPos.y,0}, canvasSize.x, canvasSize.y);
    beginScene(RenderMode::NO_DEPTH);
    uiState->id = 1;
    uiState->hot = 0;
    //uiState->active = 0;
}

void endUiFrame(){
    //endUIRender();
    endScene();
}

bool pointRectIntersection(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size){
    return  mousePos.x >= pos.x && mousePos.x <= pos.x + size.x &&
            mousePos.y >= pos.y && mousePos.y <= pos.y + size.y;
}

void UiSetHot(uint32_t buttonId){
    if(uiState->active == 0 || uiState->active == buttonId){
        uiState->hot = buttonId;
    }
}

bool UiButton(String8 text, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation){
    bool result = false;
    glm::vec2 mousePos = uiState->mousePos;
    //glm::vec2 scale = {640.0f / (float)screenWidth, 320.0f / (float)screenHeight};
    //mousePos = mousePos * scale;
    //LOGINFO("%f / %f", mousePos.x, mousePos.y);
    uint32_t buttonId = uiState->id++;

    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + size.y)};

    glm::vec4 color =  {0.0f, 0.0f, 0.0f, 0.5f};


    if(pointRectIntersection(mousePos, screenPos, size)){
        UiSetHot(buttonId);
    }
    if(uiState->active == buttonId){
        if(isMouseButtonRelease(MOUSE_BUTTON_1)){
            if(uiState->hot == buttonId){
                result = true;
            }
            uiState->active = 0;
        }
        color =  {1.0f, 0.0f, 0.0f, 0.5f};
    }else if(uiState->hot == buttonId){
        if(isMouseButtonJustPressed(MOUSE_BUTTON_1)){
            uiState->active = buttonId;
        }
        color =  {1.0f, 1.0f, 0.0f, 0.5f};
    }

    //uint32_t textWidth = calculateTextWidth(uiState->font, text, 0.3f);
    //uint32_t totalSize = 0;
    //std::vector<std::string> lines;
    //std::string line = "";
    //if(textWidth > (uint32_t)size.x){
    //    std::vector<std::string> words = splitText(text, ' ');
    //    for(size_t i = 0; i < words.size(); i++){
    //        totalSize += calculateTextWidth(uiState->font, words[i].c_str(), 0.3f);
    //        if(totalSize < (uint32_t)size.x){
    //            //strcpy(line.c_str(), words[i].c_str());
    //            //line.append(words[i]);
    //            line = line + words[i].c_str();
    //            lines.push_back(line);
    //        }else{
    //            lines.push_back(line);
    //        }
    //    }
    //}
    //renderDrawFilledRect(screenPos, size, rotation, color);
    UiText(text, pos, 0.3f);
    //for(size_t i = 0; i < lines.size(); i++){
    //    pos.y = pos.y + (i * uiState->font->characters->Size.y * 0.3f);
    //    UiText(lines[i].c_str(), pos, 0.3f);
    //}
    return result;
}

void UiText(String8 text, glm::vec2 pos, float scale){
    //float baselineY = uiState->canvasSize.y - (pos.y + uiState->font->ascender * scale);
    //glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + uiState->font->maxHeight * 0.5 * scale)};// + (uiState->font->characters->Size.y * scale))};
    //glm::vec2 screenPos = {pos.x, baselineY};
    renderDrawText2D(uiState->font, text.str, pos, scale);
}

//void UiImage(Texture* texture, glm::vec2 pos, glm::vec2 rotation){
//    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + texture->height)};
//    Rect sourceRect = {.pos = {0,0}, .size = {texture->width, texture->height}};
//    renderDrawQuad2D(texture, screenPos, {1,1}, rotation);
//}

//void UiImage(Texture* texture, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation, glm::vec2 index, glm::vec2 offset){
//    glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + texture->height)};
//    renderDrawQuad2D(texture, screenPos, size, rotation, index, offset);
//}

int calculateTextHeight(Font* font, const char* text, float scale){
    int result = 0;
    for(int i = 0; text[i] != '\0'; i++){
        int newResult = font->characters[(unsigned char)text[i]].Size.y * scale;
        if(newResult > result){
            result = newResult;
        }
    }
    return result;
}


//Deprecated
void UiText(const char* text, glm::vec2 pos, float scale){
    //float baselineY = uiState->canvasSize.y - (pos.y + uiState->font->ascender * scale);
    //glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + uiState->font->maxHeight * 0.5 * scale)};// + (uiState->font->characters->Size.y * scale))};
    //glm::vec2 screenPos = {pos.x, baselineY};
    renderDrawText2D(uiState->font, text, pos, scale);
}
//Deprecated
bool UiButton(const char* text, glm::vec2 pos, glm::vec2 size, glm::vec2 rotation){
    bool result = false;
    glm::vec2 mousePos = uiState->mousePos;
    //glm::vec2 scale = {640.0f / (float)screenWidth, 320.0f / (float)screenHeight};
    //mousePos = mousePos * scale;
    //LOGINFO("%f / %f", mousePos.x, mousePos.y);
    uint32_t buttonId = uiState->id++;

    //glm::vec2 screenPos = {pos.x, uiState->canvasSize.y - (pos.y + size.y)};

    glm::vec4 color =  {0.0f, 0.0f, 0.0f, 0.5f};


    if(pointRectIntersection(mousePos, pos, size)){
        UiSetHot(buttonId);
    }
    if(uiState->active == buttonId){
        if(isMouseButtonRelease(MOUSE_BUTTON_1)){
            if(uiState->hot == buttonId){
                result = true;
            }
            uiState->active = 0;
        }
        color =  {1.0f, 0.0f, 0.0f, 0.5f};
    }else if(uiState->hot == buttonId){
        if(isMouseButtonJustPressed(MOUSE_BUTTON_1)){
            uiState->active = buttonId;
        }
        color =  {1.0f, 1.0f, 0.0f, 0.5f};
    }

    //uint32_t textWidth = calculateTextWidth(uiState->font, text, 0.3f);
    //uint32_t totalSize = 0;
    //std::vector<std::string> lines;
    //std::string line = "";
    //if(textWidth > (uint32_t)size.x){
    //    std::vector<std::string> words = splitText(text, ' ');
    //    for(size_t i = 0; i < words.size(); i++){
    //        totalSize += calculateTextWidth(uiState->font, words[i].c_str(), 0.3f);
    //        if(totalSize < (uint32_t)size.x){
    //            //strcpy(line.c_str(), words[i].c_str());
    //            //line.append(words[i]);
    //            line = line + words[i].c_str();
    //            lines.push_back(line);
    //        }else{
    //            lines.push_back(line);
    //        }
    //    }
    //}
    //renderDrawFilledRect(screenPos, size, rotation, color);
    renderDrawFilledRect(pos, size, 0, color);
    //TODO: position text based on input (top, left), (center, center) ...
    uint32_t textHeight = calculateTextHeight(uiState->font, text, 1.0f);
    glm::vec2 textPos = {pos.x, pos.y + size.y - textHeight}; //NOTE: Text always on top
    UiText(text, textPos, 1.0f);
    //for(size_t i = 0; i < lines.size(); i++){
    //    pos.y = pos.y + (i * uiState->font->characters->Size.y * 0.3f);
    //    UiText(lines[i].c_str(), pos, 0.3f);
    //}
    return result;
}