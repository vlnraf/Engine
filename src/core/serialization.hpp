#pragma once

#include "ecs.hpp"
#include <stdio.h>

#define BUFFER_SIZE 4096

struct SerializationState{
    char buffer[BUFFER_SIZE];
    const char* filePath;
    size_t bufPos;
    int indentLevel;

    int indentList;
};

SerializationState initSerializer(const char* filePath);
void serializeWriteFile(SerializationState* state);
void serializeVec2(SerializationState* state, const char* name, const glm::vec2* v);
void serializeVec3(SerializationState* state, const char* name, const glm::vec3* v);
void serializeBool(SerializationState* state, const char* name, const bool v);
void serializeInt(SerializationState* state, const char* name, const int v);
void serializeFloat(SerializationState* state, const char* name, const float v);
void serializeString(SerializationState* state, const char* name, const char* v);
void serializeObjectStart(SerializationState* state, const char* name);
void serializeObjectEnd(SerializationState* state);
void serializeListStart(SerializationState* state, const char* name);
void serializeListEnd(SerializationState* state);
void serializeItemsStart(SerializationState* state);
void serializeItemsEnd(SerializationState* state);


//-------------------------------Parser-Deserializer---------------------------------------

struct Node{
    std::string key;
    std::string value;
    std::vector<Node> childrens;
    bool isList = false;
};

Node serializeReadFile(const char* filePath);
Node* getNode(Node* node, const char* nodeName);