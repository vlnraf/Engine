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
}

Ecs* initEcs(Arena* arena){
    //Ecs* ecs = new Ecs();
    Ecs* ecs = arenaAllocStructZero(arena, Ecs);
    Arena* ecsArena = initArena(GB(2));
    ecs->arena = ecsArena;

    ecs->entities = 0;

    ecs->sparse = arenaAllocArrayZero(ecs->arena, SparseSet, MAX_COMPONENT_TYPE);
    ecs->denseToSparse = arenaAllocArrayZero(ecs->arena, DenseToSparse, MAX_COMPONENT_TYPE);
    //ecs->componentRegistry = arenaAllocStructZero(ecsArena, ComponentRegistry);

    //ecs->componentRegistry->components = arenaAllocArrayZero(ecsArena, size_t, MAX_COMPONENT_TYPE);
    //ecs->componentRegistry->componentsCount = 0;
    //ecs->componentRegistry->componentsSize = MAX_COMPONENT_TYPE;

    ecs->removedEntities =  arenaAllocArrayZero(ecs->arena, size_t, MAX_ENTITIES);
    ecs->removedEntitiesCount = 0;

    ecs->componentId = 1; // we will use 0 as invalid component

    return ecs;
}

int hashComponent(const char* name){
    uint32_t result;
    //cast to unsigned char so i can do math operations on it
    const unsigned char* nameT = (unsigned char*) name;
    const uint32_t multiplier = 97;
    result = nameT[0] * multiplier; //multiply with prime number (reduce collisions)

    for(int i = 1; name[i] != '\0'; i++){
        result = result * multiplier + nameT[i];
    }

    result = result % MAX_COMPONENT_TYPE;
    return result;
}

int getIdForString(Ecs* ecs, const char *str) {

    // Check if string already exists
    for (size_t i = 0; i < ecs->componentId; i++) {
        if (strcmp(ecs->names[i], str) == 0) {
            return i; // existing ID
        }
    }

    // New string → store and assign new ID
    if (ecs->componentId < MAX_COMPONENT_TYPE) {
        strncpy(ecs->names[ecs->componentId], str, 500 - 1);
        ecs->names[ecs->componentId][500 - 1] = '\0';
        return ecs->componentId++;
    } else {
        return 0; // no space left
    }
}

size_t registerComponentImpl(Ecs* ecs, const char* name, const size_t size){

    //Components c = initComponents(size);
    Components c = initComponents(ecs->arena, size);
    //size_t componentType = ecs->componentId;
    //size_t componentType = getIdForString(ecs, componentName);
    size_t componentType = ecs->componentId++;

    ecs->denseToSparse[componentType].entity = arenaAllocArray(ecs->arena, int, MAX_ENTITIES);
    ecs->denseToSparse[componentType].entityCount = 0;
    ecs->denseToSparse[componentType].entitySize = MAX_ENTITIES;


    ecs->sparse[componentType].entityToComponent = arenaAllocArray(ecs->arena, int, MAX_ENTITIES);
    for(int i = 0; i < MAX_ENTITIES; i++){
        ecs->sparse[componentType].entityToComponent[i] = -1;
        ecs->denseToSparse[componentType].entity[i] = -1;
    }
    //int index = hashComponentName(componentName);
    //if(ecs->componentRegistry->components[index] > 0){
    //    LOGERROR("Collisione tra due componenti diversi, cambiare il nome di uno dei componenti altrimenti il primo inserito verra sovrascritto dall'ultimo");
    //    LOGERROR("%s", componentName);
    //}
    //ecs->componentRegistry->components[index] = componentType;
    //ecs->componentRegistry->componentsCount++;

    ecs->sparse[componentType].components = c;
    return componentType;
}

void pushComponentImpl(Ecs* ecs, const Entity id, const size_t type, const void* data){
    //int index = hashComponentName(componentName);
    //size_t componentType = ecs->componentRegistry->components[index];
    //int componentType = getIdForString(ecs, componentName);
    size_t componentType = type; 
    if(!componentType){
        LOGERROR("No component registered with name %s", type);
        return;
    }

    push_back(&ecs->sparse[componentType].components, data);
    ecs->sparse[componentType].entityToComponent[id] = ecs->sparse[componentType].components.count-1;
    ecs->denseToSparse[componentType].entity[ecs->denseToSparse[componentType].entityCount++] = id;
};

Entity createEntity(Ecs* ecs){
    Entity id;
    if(ecs->removedEntitiesCount > 0){
        size_t entityIdx = ecs->removedEntitiesCount - 1;
        id = ecs->removedEntities[entityIdx];
        ecs->removedEntitiesCount--;
    }else{
        id = ecs->entities;
        ecs->entities++;
    }
    return id;
}


bool hasComponentImpl(Ecs* ecs, const Entity entity, const size_t type){
    //size_t componentIdx = hashComponentName(componentName);
    //size_t componentType = ecs->componentRegistry->components[componentIdx];
    int componentType = type;

    if(!componentType){
        LOGERROR("No component registered with name %u", type);
        return false;
    }

    if(ecs->sparse[componentType].entityToComponent[entity] >= 0){
        return true;
    }else{
        return false;
    }
}
struct TokenArray {
    char tokens[MAX_COMPONENT_TYPE][200];
    size_t count;
};

TokenArray tokenizeText(char* text, char delimiter){
    //std::vector<std::string> result;
    //char result[MAX_COMPONENT_TYPE][200]; //200 is max length for component name;
    TokenArray result = {};
    char name[200];
    int j = 0;

    for(int i = 0 ; (text[i] != '\0'); i++){
        if(text[i] == ' '){
            continue;
        }
        if(text[i] != delimiter){
            name[j] = text[i];
            j++;
        }else{
            name[j] = '\0';
            //result.push_back(name);
            strcpy(result.tokens[result.count], name);
            result.count++;
            j=0;
        }
    }
    name[j] = '\0';
    //result.push_back(name);
    strcpy(result.tokens[result.count], name);
    result.count++;
    return result;
}

EntityArray viewImpl(Ecs* ecs, uint32_t count, uint32_t* types){
    EntityArray entities = {};
    entities.count = 0;
    //va_start(args, ecs);
    //char* inputText = va_arg(args, char*);
    //va_end(args);
    //std::vector<std::string> names = tokenizeText(inputText, ',');
    //TokenArray names = tokenizeText(inputText, ',');
    size_t smallestComponents = MAX_COMPONENTS;
    size_t componentTypeToUse = 0;
    //std::vector<size_t> componentTypes;
    size_t componentTypesCount = 0;
    size_t componentTypes[MAX_COMPONENT_TYPE];
    //for(std::string componentName : names){
    for(size_t i = 0; i < count; i++){
        //size_t componentIdx = hashComponentName(componentName.c_str());
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        //int componentType = getIdForString(ecs, names.tokens[i]);
        int componentType = types[i];
        if(ecs->sparse[componentType].components.count < smallestComponents){
            smallestComponents = ecs->sparse[componentType].components.count;
            componentTypeToUse = componentType;
        }
        //componentTypes.push_back(componentType);
        //componentTypes[componentTypesCount++] = componentType;
    }
    //if(componentTypeToUse == 0){
    //    LOGERROR("Passed a component id that does not exist!!");
    //    return entities;
    //}

    for(size_t i = 0; i < smallestComponents; i++){
        int entity = ecs->denseToSparse[componentTypeToUse].entity[i];
        bool hasAll = true;
        //for(size_t componentType : componentTypes){
        for(size_t j = 0; j < count; j++){
            size_t componentType = types[j];
            if(componentType == componentTypeToUse) continue;
            if(ecs->sparse[componentType].entityToComponent[entity] == -1){
                hasAll = false;
                break;
            }
        }
        if(hasAll){
            entities.entities[entities.count++] = entity;
            //entities.push_back(entity);
        }
    }
    return entities;
}

//TODO: remove the std vector and use an array
EntityArray view2(Ecs* ecs, size_t* types, size_t count){
    //va_list args;
    //std::vector<Entity> entities;
    EntityArray entities = {};
    entities.count = 0;
    //va_start(args, ecs);
    //char* inputText = va_arg(args, char*);
    //va_end(args);
    //std::vector<std::string> names = tokenizeText(inputText, ',');
    //TokenArray names = tokenizeText(inputText, ',');
    size_t smallestComponents = MAX_COMPONENTS;
    size_t componentTypeToUse = 0;
    //std::vector<size_t> componentTypes;
    size_t componentTypesCount = 0;
    size_t componentTypes[MAX_COMPONENT_TYPE];
    //for(std::string componentName : names){
    for(size_t i = 0; i < count; i++){
        //size_t componentIdx = hashComponentName(componentName.c_str());
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        //int componentType = getIdForString(ecs, names.tokens[i]);
        int componentType = types[i];
        if(ecs->sparse[componentType].components.count < smallestComponents){
            smallestComponents = ecs->sparse[componentType].components.count;
            componentTypeToUse = componentType;
        }
        //componentTypes.push_back(componentType);
        componentTypes[componentTypesCount++] = componentType;
    }

    for(size_t i = 0; i < smallestComponents; i++){
        int entity = ecs->denseToSparse[componentTypeToUse].entity[i];
        bool hasAll = true;
        //for(size_t componentType : componentTypes){
        for(size_t j = 0; j < componentTypesCount; j++){
            size_t componentType = componentTypes[j];
            if(componentType == componentTypeToUse) continue;
            if(ecs->sparse[componentType].entityToComponent[entity] == -1){
                hasAll = false;
                break;
            }
        }
        if(hasAll){
            entities.entities[entities.count++] = entity;
            //entities.push_back(entity);
        }
    }
    return entities;
}

void* getComponentImpl(Ecs* ecs, Entity entity, const size_t type){
    if(hasComponentImpl(ecs, entity, type)){
        //size_t componentIdx = hashComponentName(componentName);
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        //int componentType = getIdForString(ecs, componentName);
        int componentType = type;
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
        //size_t componentIdx = hashComponentName(componentName);
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        //int componentType = getIdForString(ecs, componentName);
        int componentType = type;
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

//bool hasComponentFromIndex(Ecs* ecs, const Entity entity, size_t componentIdx){
//    //size_t componentType = ecs->componentRegistry.at(componentName);
//    //size_t componentType = ecs->componentRegistry->components[componentIdx];
//
//    if(!componentType){
//        return false;
//    }
//
//    if(ecs->sparse[componentType].entityToComponent[entity] >= 0){
//        return true;
//    }else{
//        return false;
//    }
//}
//
//void removeComponentFromIndex(Ecs* ecs, Entity entity, size_t componentIdx){
//    if(hasComponentFromIndex(ecs, entity, componentIdx)){
//        size_t componentType = ecs->componentRegistry->components[componentIdx];
//        uint32_t denseIndex = ecs->sparse[componentType].entityToComponent[entity];
//        if(ecs->sparse[componentType].components.count == 0){
//            return;
//        }
//        uint32_t denseLast = ecs->sparse[componentType].components.count-1;
//
//        uint32_t backEntity = ecs->denseToSparse[componentType].entity[denseLast];
//
//        if(denseIndex != denseLast){
//            void* swapComp = get(&ecs->sparse[componentType].components, denseLast);
//            if(swapComp){
//                insert(&ecs->sparse[componentType].components, denseIndex, swapComp);
//                ecs->sparse[componentType].entityToComponent[backEntity] = denseIndex;
//                ecs->denseToSparse[componentType].entity[denseIndex] = backEntity;
//            }else{
//                return;
//            }
//        }
//        pop_back(&ecs->sparse[componentType].components);
//        ecs->denseToSparse[componentType].entityCount--;
//        ecs->sparse[componentType].entityToComponent[entity] = -1;
//    }
//}

void removeEntity(Ecs* ecs, Entity entity){
    //for(size_t i = 0; i < ecs->componentRegistry->componentsSize; i++){
    //    size_t componentType = ecs->componentRegistry->components[i];
    //    if(componentType != 0) {
    //        removeComponentFromIndex(ecs, entity, i);
    //    }
    //}
    for(size_t i = 1; i < ecs->componentId; i++){
        removeComponentImpl(ecs, entity, i);//ecs->names[i]);
    }
    ecs->removedEntities[ecs->removedEntitiesCount++] = entity;
}

void clearEcs(Ecs* ecs){
    for(size_t entity = 0; entity < ecs->entities; entity++){
        //if(hasComponent(ecs, entity, PersistentTag)){
        //    continue;
        //}
        removeEntity(ecs, entity);
    }
}

void destroyEcs(Ecs* ecs){
    clearArena(ecs->arena);
    free(ecs->arena);
}