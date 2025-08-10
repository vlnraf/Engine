#include "ecs.hpp"
#include "tracelog.hpp"
#include "profiler.hpp"

#include <string.h>
#include <stdio.h>

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

Ecs* initEcs(Arena* arena){
    //Ecs* ecs = new Ecs();
    Ecs* ecs = arenaAllocStructZero(arena, Ecs);
    Arena* ecsArena = initArena(MB(100));
    ecs->arena = ecsArena;

    ecs->entities = 0;

    ecs->sparse = arenaAllocArrayZero(ecsArena, SparseSet, MAX_COMPONENT_TYPE);
    ecs->denseToSparse = arenaAllocArrayZero(ecsArena, DenseToSparse, MAX_COMPONENT_TYPE);
    ecs->componentRegistry = arenaAllocStructZero(ecsArena, ComponentRegistry);

    ecs->componentRegistry->components = arenaAllocArrayZero(ecsArena, size_t, MAX_COMPONENT_TYPE);
    ecs->componentRegistry->componentsCount = 0;
    ecs->componentRegistry->componentsSize = MAX_COMPONENT_TYPE;

    ecs->removedEntities =  arenaAllocArrayZero(ecsArena, size_t, MAX_ENTITIES);
    ecs->removedEntitiesCount = 0;

    ecs->componentId = 1; // we will use 0 as invalid component

    return ecs;
}

int hashComponentName(const char* name){
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

static char names[MAX_COMPONENT_TYPE][500];
static int count = 1;
int get_id_for_string(const char *str) {

    // Check if string already exists
    for (int i = 0; i < count; i++) {
        if (strcmp(names[i], str) == 0) {
            return i; // existing ID
        }
    }

    // New string → store and assign new ID
    if (count < MAX_COMPONENT_TYPE) {
        strncpy(names[count], str, 500 - 1);
        names[count][500 - 1] = '\0';
        return count++;
    } else {
        return -1; // no space left
    }
}

void registerComponentName(Ecs* ecs, const char* componentName, const size_t size){

    //Components c = initComponents(size);
    Components c = initComponents(ecs->arena, size);
    //size_t componentType = ecs->componentId;
    size_t componentType = (size_t)get_id_for_string(componentName);

    ecs->denseToSparse[componentType].entity = arenaAllocArray(ecs->arena, int, MAX_ENTITIES);
    ecs->denseToSparse[componentType].entityCount = 0;
    ecs->denseToSparse[componentType].entitySize = MAX_ENTITIES;


    ecs->sparse[componentType].entityToComponent = arenaAllocArray(ecs->arena, int, MAX_ENTITIES);
    for(int i = 0; i < MAX_ENTITIES; i++){
        ecs->sparse[componentType].entityToComponent[i] = -1;
    }
    int index = hashComponentName(componentName);
    if(ecs->componentRegistry->components[index] > 0){
        LOGERROR("Collisione tra due componenti diversi, cambiare il nome di uno dei componenti altrimenti il primo inserito verra sovrascritto dall'ultimo");
        LOGERROR("%s", componentName);
    }
    //ecs->componentRegistry->components[index] = componentType;
    //ecs->componentRegistry->componentsCount++;

    ecs->sparse[componentType].components = c;
    ecs->componentId++;
}

void pushComponentName(Ecs* ecs, const Entity id, const char* componentName, const void* data){
    //int index = hashComponentName(componentName);
    //size_t componentType = ecs->componentRegistry->components[index];
    int componentType = get_id_for_string(componentName);
    if(!componentType){
        LOGERROR("No component registered with name %s", componentName);
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


bool hasComponentName(Ecs* ecs, const Entity entity, const char* componentName){
    //size_t componentIdx = hashComponentName(componentName);
    //size_t componentType = ecs->componentRegistry->components[componentIdx];
    int componentType = get_id_for_string(componentName);

    if(!componentType){
        LOGERROR("No component registered with name %s", componentName);
        return false;
    }

    if(ecs->sparse[componentType].entityToComponent[entity] >= 0){
        return true;
    }else{
        return false;
    }
}

std::vector<std::string> tokenizeText(char* text, char delimiter){
    std::vector<std::string> result;
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
            result.push_back(name);
            j=0;
        }
    }
    name[j] = '\0';
    result.push_back(name);
    return result;
}

std::vector<Entity> viewName(Ecs* ecs, ...){
    va_list args;
    std::vector<Entity> entities;
    va_start(args, ecs);
    char* inputText = va_arg(args, char*);
    std::vector<std::string> names = tokenizeText(inputText, ',');
    size_t smallestComponents = MAX_COMPONENT_TYPE;
    size_t componentTypeToUse = 0;
    std::vector<size_t> componentTypes;
    for(std::string componentName : names){
        //size_t componentIdx = hashComponentName(componentName.c_str());
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        int componentType = get_id_for_string(componentName.c_str());
        if(ecs->sparse[componentType].components.count < smallestComponents){
            smallestComponents = ecs->sparse[componentType].components.count;
            componentTypeToUse = componentType;
        }
        componentTypes.push_back(componentType);
    }

    for(size_t i = 0; i < smallestComponents; i++){
        int entity = ecs->denseToSparse[componentTypeToUse].entity[i];
        bool hasAll = true;
        for(size_t componentType : componentTypes){
            if(componentType == componentTypeToUse) continue;
            if(ecs->sparse[componentType].entityToComponent[entity] == -1){
                hasAll = false;
                break;
            }
        }
        if(hasAll){
            entities.push_back(entity);
        }
    }
    va_end(args);
    return entities;

}

void* getComponentName(Ecs* ecs, Entity entity, const char* componentName){
    if(hasComponentName(ecs, entity, componentName)){
        //size_t componentIdx = hashComponentName(componentName);
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        int componentType = get_id_for_string(componentName);
        //NOTE: probably it's usless because already checked if it has the component name
        if(!componentType){
            LOGERROR("No component of type %s", componentName);
            return nullptr;
        }
        return get(&ecs->sparse[componentType].components, ecs->sparse[componentType].entityToComponent[entity]);
    }else{
        return nullptr;
    }
}


void removeComponentName(Ecs* ecs, Entity entity, const char* componentName){
    if(hasComponentName(ecs, entity, componentName)){
        //size_t componentIdx = hashComponentName(componentName);
        //size_t componentType = ecs->componentRegistry->components[componentIdx];
        int componentType = get_id_for_string(componentName);
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
    for(int i = 1; i < count; i++){
        removeComponentName(ecs, entity, names[i]);
    }
    ecs->removedEntities[ecs->removedEntitiesCount++] = entity;
}

void clearEcs(Ecs* ecs){
    for(size_t entity = 0; entity < ecs->entities; entity++){
        if(hasComponent(ecs, entity, PersistentTag)){
            continue;
        }
        removeEntity(ecs, entity);
    }
}

void destroyEcs(Ecs* ecs){
    clearArena(ecs->arena);
    free(ecs->arena);
}