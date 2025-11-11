#include "ecs.hpp"
#include "tracelog.hpp"
#include "profiler.hpp"

//#include <string.h>
//#include <stdio.h>

ECS_DECLARE_COMPONENT_EXTERN(TransformComponent);
ECS_DECLARE_COMPONENT_EXTERN(DirectionComponent);
ECS_DECLARE_COMPONENT_EXTERN(VelocityComponent);
ECS_DECLARE_COMPONENT_EXTERN(SpriteComponent);
ECS_DECLARE_COMPONENT_EXTERN(PersistentTag);
ECS_DECLARE_COMPONENT_EXTERN(AnimationComponent);
ECS_DECLARE_COMPONENT_EXTERN(Parent);
ECS_DECLARE_COMPONENT_EXTERN(Child);

void* back(Components* components){
    if(components->count == 0) return nullptr;
    return (void*)((char*)(components->elements) + ((components->count-1) * components->elementSize));
}

void pop_back(Components* components){
    if(components->count > 0){
        components->count--;
    }
}

void push_back(Components* components, const void* data){
    void* toInsert = ((char*)components->elements) + (components->count * components->elementSize);
    memcpy(toInsert, data, components->elementSize);
    components->count++;
}

void* get(Components* components, size_t index){
    if(index > components->count){
        LOGERROR("Out of bound");
        return NULL;
    }
    return (void*)((char*)components->elements + index * components->elementSize);
}

void insert(Components* components, size_t index, const void* data){
    void* toInsert = ((char*)components->elements) + (index * components->elementSize);
    memcpy(toInsert, data, components->elementSize);
}

Components initComponents(Arena* arena, size_t size){
    Components components = {};
    //components.elements = malloc(size * MAX_COMPONENTS);
    components.elements = arenaAlloc(arena, size * MAX_COMPONENTS);
    components.count = 0;
    components.elementSize = size;
    return components;
}

void importBaseModule(Ecs* ecs){
    registerComponent(ecs, TransformComponent);
    registerComponent(ecs, SpriteComponent);
    registerComponent(ecs, DirectionComponent);
    registerComponent(ecs, VelocityComponent);
    registerComponent(ecs, PersistentTag);
    registerComponent(ecs, AnimationComponent);
    registerComponent(ecs, Parent);
    registerComponent(ecs, Child);
}

Ecs* initEcs(Arena* arena){
    //Ecs* ecs = new Ecs();
    Ecs* ecs = arenaAllocStructZero(arena, Ecs);
    Arena* ecsArena = initArena(GB(1));
    ecs->arena = ecsArena;

    ecs->entities = 0;

    ecs->sparse = arenaAllocArrayZero(ecs->arena, SparseSet, MAX_COMPONENT_TYPE);
    ecs->denseToSparse = arenaAllocArrayZero(ecs->arena, DenseToSparse, MAX_COMPONENT_TYPE);

    ecs->removedEntities =  arenaAllocArrayZero(ecs->arena, size_t, MAX_ENTITIES);
    ecs->removedEntitiesCount = 0;

    ecs->componentId = 1; // we will use 0 as invalid component

    return ecs;
}

int getIdForString(Ecs* ecs, const char *str) {
    // Check if string already exists
    for (size_t i = 1; i < ecs->componentId; i++) {
        if (strcmp(ecs->names[i], str) == 0) {
            return i; // existing ID
        }
    }
    return 0;
}

size_t registerComponentImpl(Ecs* ecs, const char* name, const size_t size){

    size_t componentType = getIdForString(ecs, name);
    if(!componentType){
        componentType = ecs->componentId++;
        strncpy(ecs->names[componentType], name, 500 - 1);
        ecs->names[componentType][500 - 1] = '\0';
    }else{
        return componentType;
    }
    Components c = initComponents(ecs->arena, size);

    ecs->denseToSparse[componentType].entity = arenaAllocArray(ecs->arena, uint32_t, MAX_ENTITIES);
    ecs->denseToSparse[componentType].entityCount = 0;
    ecs->denseToSparse[componentType].entitySize = MAX_ENTITIES;


    ecs->sparse[componentType].entityToComponent = arenaAllocArray(ecs->arena, uint32_t, MAX_ENTITIES);
    for(size_t i = 0; i < MAX_ENTITIES; i++){
        ecs->sparse[componentType].entityToComponent[i] = NULL_ENTITY;
        ecs->denseToSparse[componentType].entity[i] = NULL_ENTITY;
    }

    ecs->sparse[componentType].components = c;
    return componentType;
}

void pushComponentImpl(Ecs* ecs, const Entity id, const size_t type, const void* data){
    size_t componentType = type; 
    if(!componentType){
        LOGERROR("No component registered with name %u", type);
        return;
    }

    push_back(&ecs->sparse[componentType].components, data);
    ecs->sparse[componentType].entityToComponent[id] = ecs->sparse[componentType].components.count-1;
    ecs->denseToSparse[componentType].entity[ecs->denseToSparse[componentType].entityCount++] = id;
    //If i am adding the parent component i also update the childs of the parents
    if(type == ECS_TYPE(Parent)){
        Parent* p = getComponent(ecs, id, Parent);
        if(p){
            if(hasComponent(ecs, p->entity, Child)){
                Child* child = getComponent(ecs, p->entity, Child);
                child->entity[child->count++] = id;
            }else{
                Child child = {};
                child.entity[child.count++] = id;
                pushComponent(ecs, p->entity, Child, &child);
            }
        }
    }
};

Entity createEntity(Ecs* ecs){
    Entity id;
    if(ecs->removedEntitiesCount > 0){
        size_t entityIdx = ecs->removedEntitiesCount - 1;
        id = ecs->removedEntities[entityIdx];
        ecs->removedEntitiesCount--;
        ecs->entitiesCount++;
    }else{
        id = ecs->entities;
        ecs->entities++;
        ecs->entitiesCount++;
    }
    return id;
}


bool hasComponentImpl(Ecs* ecs, const Entity entity, const size_t type){
    uint32_t componentType = type;

    if(!componentType){
        LOGERROR("No component registered with name %u", type);
        return false;
    }

    if(ecs->sparse[componentType].entityToComponent[entity] != NULL_ENTITY){
        return true;
    }else{
        return false;
    }
}

EntityArray viewImpl(Ecs* ecs, uint32_t count, uint32_t* types){
    EntityArray entities = {};
    entities.count = 0;
    size_t smallestComponents = MAX_COMPONENTS;
    size_t componentTypeToUse = 0;
    size_t componentTypesCount = 0;
    //size_t componentTypes[MAX_COMPONENT_TYPE];
    for(size_t i = 0; i < count; i++){
        int componentType = types[i];
        if(ecs->sparse[componentType].components.count < smallestComponents){
            smallestComponents = ecs->sparse[componentType].components.count;
            componentTypeToUse = componentType;
        }
    }

    for(size_t i = 0; i < smallestComponents; i++){
        uint32_t entity = ecs->denseToSparse[componentTypeToUse].entity[i];
        bool hasAll = true;
        for(size_t j = 0; j < count; j++){
            size_t componentType = types[j];
            if(componentType == componentTypeToUse) continue;
            //if(ecs->sparse[componentType].entityToComponent[entity] == NULL_ENTITY){
            if(!hasComponentImpl(ecs, entity, componentType)){
                hasAll = false;
                break;
            }
        }
        if(hasAll){
            entities.entities[entities.count++] = entity;
        }
    }
    return entities;
}

void* getComponentImpl(Ecs* ecs, Entity entity, const size_t type){
    if(!hasComponentImpl(ecs, entity, type)){
        if(type == 9){
            LOGINFO("WHAT A FUCK");
        }
    }
    if(hasComponentImpl(ecs, entity, type)){
        //size_t componentIdx = hashComponentName(componentName);
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        //int componentType = getIdForString(ecs, componentName);
        uint32_t componentType = type;
        //NOTE: probably it's usless because already checked if it has the component name
        if(!componentType){
            LOGERROR("No component of type %u", type);
            return nullptr;
        }
        return get(&ecs->sparse[componentType].components, ecs->sparse[componentType].entityToComponent[entity]);
    }else{
        return nullptr;
    }
}


void removeComponentImpl(Ecs* ecs, Entity entity, const size_t type){
    if(hasComponentImpl(ecs, entity, type)){
        uint32_t componentType = type;
        uint32_t denseIndex = ecs->sparse[componentType].entityToComponent[entity];
        if(ecs->sparse[componentType].components.count == 0){
            return;
        }
        uint32_t denseLast = ecs->sparse[componentType].components.count-1;

        uint32_t backEntity = ecs->denseToSparse[componentType].entity[denseLast];

        if(denseIndex != denseLast){
            void* swapComp = get(&ecs->sparse[componentType].components, denseLast);
            if(swapComp){
                insert(&ecs->sparse[componentType].components, denseIndex, swapComp);
                ecs->sparse[componentType].entityToComponent[backEntity] = denseIndex;
                ecs->denseToSparse[componentType].entity[denseIndex] = backEntity;
            }else{
                return;
            }
        }
        pop_back(&ecs->sparse[componentType].components);
        ecs->denseToSparse[componentType].entity[ecs->denseToSparse[componentType].entityCount-1] = -1;
        ecs->denseToSparse[componentType].entityCount--;
        ecs->sparse[componentType].entityToComponent[entity] = -1;
    }
}


void removeEntity(Ecs* ecs, Entity entity){
    for(size_t i = 1; i < ecs->componentId; i++){
        removeComponentImpl(ecs, entity, i);
    }
    ecs->removedEntities[ecs->removedEntitiesCount++] = entity;
    ecs->entitiesCount--;
}

void clearEcs(Ecs* ecs){
    for(size_t entity = 0; entity < ecs->entities; entity++){
        removeEntity(ecs, entity);
    }
}

void destroyEcs(Ecs* ecs){
    clearArena(ecs->arena);
    free(ecs->arena);
}