#include "uirenderer.hpp"

//UIRenderer* uiRenderer = nullptr;
static UIRenderer* uiRenderer = nullptr;

void initUIRenderer(const uint32_t width, const uint32_t height){
    uiRenderer = new UIRenderer();
    uiRenderer->width = width;
    uiRenderer->height = height;
    glViewport(0, 0, width, height);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    genVertexArrayObject(&uiRenderer->uiVao);
    genVertexBuffer(&uiRenderer->uiVbo);
    genVertexArrayObject(&uiRenderer->textVao);
    genVertexBuffer(&uiRenderer->textVbo);
}

void destroyUIRenderer(){
    delete uiRenderer;
}

void pushButton(const glm::vec2 pos, const glm::vec2 size){
    //Bot left origin
    size_t verticesSize = 6;
    glm::vec3 vertexPosition[] = {{0.0f, 1.0f, 0.0f},
                                  {1.0f, 0.0f, 0.0f},
                                  {0.0f, 0.0f, 0.0f}, 
                                  {0.0f, 1.0f, 0.0f},
                                  {1.0f, 1.0f, 0.0f},
                                  {1.0f, 0.0f, 0.0f}};

    for(size_t i = 0; i < verticesSize; i++){
        UIVertex v = {};
        v.pos = vertexPosition[i];
        v.pos.x = v.pos.x * size.x + pos.x;
        v.pos.y = v.pos.y * size.y + pos.y;
        v.color = glm::vec4(0.5, 0.5, 0.5, 0.75);
        uiRenderer->vertices.push_back(v);
    }
}

void pushText(const char* text, glm::vec2 pos, const float scale){
    for(int i = 0; text[i] != '\0'; i++){
        Character ch = uiRenderer->uiFont->characters[(unsigned char) text[i]];

        float xpos = pos.x;
        float ypos = pos.y;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        glm::vec4 uv = calculateUV(uiRenderer->uiFont->texture, {0,0}, {ch.Size.x, ch.Size.y},{ch.xOffset, 0}); //index is always 0 because the character size change and so we can't rely on index
        // update VBO for each character
        glm::vec4 verticesPosition[6] = {
            {xpos,     ypos + h, 0, 1.0f}, //uv.y, uv.z,//0.0f, 0.0f ,            
            {xpos,     ypos    , 0, 1.0f}, //uv.y, uv.x,//0.0f, 1.0f ,
            {xpos + w, ypos    , 0, 1.0f}, //uv.w, uv.x,//1.0f, 1.0f ,
            {xpos,     ypos + h, 0, 1.0f}, //uv.y, uv.z, //0.0f, 0.0f ,
            {xpos + w, ypos    , 0, 1.0f}, //uv.w, uv.x, //1.0f, 1.0f ,
            {xpos + w, ypos + h, 0, 1.0f} //uv.w, uv.z
        };
        glm::vec2 textureCoords[6] = {
            {uv.y, uv.z},//0.0f, 0.0f ,            
            {uv.y, uv.x},//0.0f, 1.0f ,
            {uv.w, uv.x},//1.0f, 1.0f ,
            {uv.y, uv.z}, //0.0f, 0.0f ,
            {uv.w, uv.x}, //1.0f, 1.0f ,
            {uv.w, uv.z}
        };

        for(size_t i = 0; i < 6; i++){
            QuadVertex v = {};
            v.pos = verticesPosition[i];
            v.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
            v.texCoord = textureCoords[i];
            uiRenderer->textVertices.push_back(v);
        }
        pos.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

void renderUIElements(){
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 uiProjection = glm::ortho(0.0f, (float)uiRenderer->width, 0.0f, (float)uiRenderer->height, -1.0f, 1.0f);

    //BUTTONS data
    bindVertexArrayObject(uiRenderer->uiVao);
    bindVertexArrayBuffer(uiRenderer->uiVbo, uiRenderer->vertices.data(), uiRenderer->vertices.size());
    useShader(&uiRenderer->uiShader);
    setUniform(&uiRenderer->uiShader, "projection", uiProjection);

    commandDrawQuad(uiRenderer->vertices.data(), uiRenderer->vertices.size());

    //TEXT data
    bindVertexArrayObject(uiRenderer->textVao);
    bindVertexArrayBuffer(uiRenderer->textVbo, uiRenderer->textVertices.data(), uiRenderer->textVertices.size());
    useShader(&uiRenderer->uiTextShader);
    setUniform(&uiRenderer->uiTextShader, "projection", uiProjection);
    setUniform(&uiRenderer->uiTextShader, "textColor", glm::vec3(1,1,1)); //TODO: take color as input
    //commandDrawQuad(uiRenderer->uiFont->texture->id, uiRenderer->textVertices.data(), uiRenderer->textVertices.size());
}