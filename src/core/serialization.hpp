#pragma once

#include "ecs.hpp"
#include "core/coreapi.hpp"
#include <stdio.h>
#include <string>

#define BUFFER_SIZE 4096

struct SerializationState{
    char buffer[BUFFER_SIZE];
    const char* filePath;
    size_t bufPos;
    int indentLevel;

    int indentList;
};

CORE_API SerializationState initSerializer(const char* filePath);
CORE_API void serializeWriteFile(SerializationState* state);
CORE_API void serializeVec2(SerializationState* state, const char* name, const glm::vec2* v);
CORE_API void serializeVec3(SerializationState* state, const char* name, const glm::vec3* v);
CORE_API void serializeBool(SerializationState* state, const char* name, const bool v);
CORE_API void serializeInt(SerializationState* state, const char* name, const int v);
CORE_API void serializeFloat(SerializationState* state, const char* name, const float v);
CORE_API void serializeString(SerializationState* state, const char* name, const char* v);
CORE_API void serializeObjectStart(SerializationState* state, const char* name);
CORE_API void serializeObjectEnd(SerializationState* state);
CORE_API void serializeListStart(SerializationState* state, const char* name);
CORE_API void serializeListEnd(SerializationState* state);
CORE_API void serializeItemsStart(SerializationState* state);
CORE_API void serializeItemsEnd(SerializationState* state);


//-------------------------------Parser-Deserializer---------------------------------------

struct Node{
    std::string key;
    std::string value;
    std::vector<Node> childrens;
    bool isList = false;
};

CORE_API Node serializeReadFile(const char* filePath);
CORE_API Node* getNode(Node* node, const char* nodeName);