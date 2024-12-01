#include "ecs.hpp"
#include "tracelog.hpp"

typedef void(*ComponentInitializer)(Ecs*, Entity, void*);

ComponentInitializer componentInitializers[] = {
    initTransform,  // ECS_TRANSFORM
    initSprite,     // ECS_HEALTH
};

Ecs* initEcs(){
    Ecs* ecs = new Ecs();
    ecs->entities = 0;
    return ecs;
}

void pushComponent(Ecs* ecs, int id, ComponentType type){
    if(type < COMPONENT_TYPE_COUNT && componentInitializers[type]){
        componentInitializers[type](ecs, id, nullptr);
        ecs->entityComponentMap[id].insert(type);
    }else{
        LOGERROR("Unknown component type: %d", type);
    }
}

void pushComponent(Ecs* ecs, int id, ComponentType type, void* components){
    if(type < COMPONENT_TYPE_COUNT && componentInitializers[type]){
        componentInitializers[type](ecs, id, components);
        ecs->entityComponentMap[id].insert(type);
    }else{
        LOGERROR("Unknown component type: %d", type);
    }
}

uint32_t createEntity(Ecs* ecs, std::vector<ComponentType> types, std::vector<void*> components){
    Entity entityId = ecs->entities;
    for(int i = 0; i < types.size(); i++){
        if(components.size() > 0){
            pushComponent(ecs, entityId, types[i], components[i]);
        }else{
            pushComponent(ecs, entityId, types[i], nullptr); 
        }
    }
    ecs->entities++;
    return entityId;
}

std::vector<Entity> view(Ecs* ecs, const:: std::vector<ComponentType> requiredComponents){
    std::vector<Entity> matchingEntities;

    for(int i = 0; i < ecs->entities; i++){
        uint32_t entityId = i;
        std::unordered_set<ComponentType> components = ecs->entityComponentMap[i];
        bool matches = true;
        for(int j = 0; j < requiredComponents.size(); j++){
            if(components.find(requiredComponents[j]) == components.end()){ // not found in the set
                matches = false;
                break;
            }
        }
        if(matches){
            matchingEntities.push_back(entityId);
        }
    }
    return matchingEntities;
}

void initTransform(Ecs* ecs, Entity id, void* components){
    if(components){
        TransformComponent* comp = (TransformComponent*) components;
        TransformComponent c = {};
        c.position = comp->position;
        c.rotation = comp->rotation;
        c.scale = comp->scale;
        c.entityId = id;
        ecs->components.transforms.push_back(c);
    }else{
        TransformComponent c = {};
        c.entityId = id;
        ecs->components.transforms.push_back(c);
    }
}

void initSprite(Ecs* ecs, Entity id, void* components){
    if(components){
        SpriteComponent* s = (SpriteComponent*) components;
        SpriteComponent sprite = {};
        sprite.texture = s->texture;
        sprite.vertCount = QUAD_VERTEX_SIZE;
        sprite.entityId = id;
        ecs->components.sprite.push_back(sprite);
    }else{
        SpriteComponent sprite = {};
        sprite.vertCount = QUAD_VERTEX_SIZE;
        sprite.texture = getWhiteTexture();

        sprite.entityId = id;
        ecs->components.sprite.push_back(sprite);
    }
}

void updateTranformers(Ecs* ecs, int id, glm::vec3 pos, glm::vec3 scale, glm::vec3 rotation){
    ecs->components.transforms[id].position = pos;
    ecs->components.transforms[id].rotation = rotation;
    ecs->components.transforms[id].scale = scale;
}