#include "ecs.hpp"
#include "tracelog.hpp"

Ecs* initEcs(uint32_t size){
    Ecs* ecs = (Ecs*)malloc(sizeof(Ecs));
    ecs->entities = 0;
    ecs->maxEntities = size;
    memset(ecs->components.input, -1, 200);
    memset(ecs->components.transforms, -1, 200);
    memset(ecs->components.h, -1, 200);
    return ecs;
}
void createEntity(Ecs* ecs, ComponentType* type, uint32_t typeSize){
    for(int i = 0; i < typeSize; i++){
        if(type[i] == TRANSFORM){
            TransformComponent c = {};
            ecs->components.transforms[ecs->entities] = c;
            ecs->components.transforms[ecs->entities].entityId = ecs->entities;
        }
        if(type[i] == HEALTH){
            HealthComponent h;
            ecs->components.h[ecs->entities] = h;
            ecs->components.h[ecs->entities].entityId = ecs->entities;
        }
        if(type[i] == INPUT){
            InputComponent input;
            ecs->components.input[ecs->entities] = input;
            ecs->components.input[ecs->entities].entityId = ecs->entities;
        }
    }
    ecs->entities++;
}

void pushComponent(Ecs* ecs, int id, ComponentType type){
        if(type == TRANSFORM){
            TransformComponent c = {};
            ecs->components.transforms[id] = c;
            ecs->components.transforms[id].entityId = id;
        }
        if(type == HEALTH){
            HealthComponent h = {};
            ecs->components.h[id] = h;
            ecs->components.h[id].entityId = id;
        }
        if(type == INPUT){
            InputComponent input = {};
            ecs->components.input[id] = input;
            ecs->components.input[id].entityId = id;
        }

}

void updateTranformers(Ecs* ecs, int id, glm::vec3 pos){
    ecs->components.transforms[id].position = pos;
}

void inputSystem(Ecs* ecs, InputComponent input){
    for(int i = 0; i < 200; i++){
        int id = ecs->components.input[i].entityId;
        if(id >= 0){
            updateTranformers(ecs, id, glm::vec3(input.x, input.y, 0.0f));
        }
    }

}

int main(){
    Ecs* ecs = initEcs(10);
    //TransformComponent component;
    //HealthComponent h;
    InputComponent input;
    ComponentType types[2] = {TRANSFORM, INPUT};
    ComponentType types1[1] = {TRANSFORM};
    ComponentType types2[2] = {TRANSFORM};
    createEntity(ecs, types, 2);
    createEntity(ecs, types1, 1);
    createEntity(ecs, types2, 2);
    pushComponent(ecs, 1, INPUT);
    input.x = 1;
    input.y = 1;
    inputSystem(ecs, input);
    for(int i = 0; i < ecs->entities; i++){
        LOGINFO("%f, %f, %f", ecs->components.transforms[i].position.x, ecs->components.transforms[i].position.y, ecs->components.transforms[i].position.z);
    }
    return 0;
}