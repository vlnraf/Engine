#include "ecs.hpp"
#include "tracelog.hpp"

typedef void(*ComponentInitializer)(Ecs*, Entity);

ComponentInitializer componentInitializers[] = {
    initTransform,  // ECS_TRANSFORM
    initSprite,     // ECS_HEALTH
    initInput,      // ECS_INPUT
};

Ecs* initEcs(){
    Ecs* ecs = new Ecs();
    ecs->entities = 0;
    return ecs;
}

void initTransform(Ecs* ecs, Entity id){
    TransformComponent c = {};
    c.entityId = id;
    ecs->components.transforms.push_back(c);
}

void initSprite(Ecs* ecs, Entity id){
    SpriteComponent sprite = {};
    sprite.vertCount = QUAD_VERTEX_SIZE;

    sprite.entityId = id;
    ecs->components.sprite.push_back(sprite);
}

void initInput(Ecs* ecs, Entity id){
    InputComponent i = {};
    i.entityId = id;
    ecs->components.input.push_back(i);
}

void pushComponent(Ecs* ecs, int id, ComponentType type){
    if(type < COMPONENT_TYPE_COUNT && componentInitializers[type]){
        componentInitializers[type](ecs, id);
    }else{
        LOGERROR("Unknown component type: %d", type);
    }
}

void createEntity(Ecs* ecs, std::vector<ComponentType> types){
    Entity entityId = ecs->entities;
    for(int i = 0; i < types.size(); i++){
        pushComponent(ecs, entityId, types[i]);
    }
    ecs->entities++;
}

void updateTranformers(Ecs* ecs, int id, glm::vec3 pos){
    ecs->components.transforms[id].position = pos;
}

void inputSystem(Ecs* ecs, InputComponent input){
    for(int i = 0; i < ecs->components.input.size(); i++){
        int id = ecs->components.input[i].entityId;
        updateTranformers(ecs, id, glm::vec3(input.x, input.y, 0.0f));
    }
}