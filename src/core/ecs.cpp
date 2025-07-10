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
    return (void*)(((char*)components->elements) + (components->elementSize * index));
}

void insert(Components* components, size_t index, const void* data){
    void* toInsert = ((char*)components->elements) + (index * components->elementSize);
    memcpy(toInsert, data, components->elementSize);
}

Components initComponents(size_t size){
    Components components = {};
    components.elements = malloc(size * MAX_COMPONENTS);
    components.count = 0;
    components.elementSize = size;
    return components;
}

Ecs* initEcs(){
    Ecs* ecs = new Ecs();
    ecs->entities = 0;

    //for(int type = 0; type < ComponentType::COMPONENT_TYPE_COUNT; type++){
        //for(int i = 0; i < MAX_ENTITIES; i++){
            //ecs->sparse[(ComponentType)type].push_back(-1);
            //ecs->denseToSparse[(ComponentType)type].push_back(-1);
        //}
    //}

    return ecs;
}

void registerComponentName(Ecs* ecs, const char* componentName, const size_t size){

    Components c = initComponents(size);
    size_t componentType = ecs->componentId;

    for(int i = 0; i < MAX_ENTITIES; i++){
        ecs->sparse[componentType].push_back(-1);
    }

    ecs->componentRegistry.insert({componentName, componentType});
    ecs->dense[componentType] = c;

    ecs->componentId++;
}

void pushComponentName(Ecs* ecs, const Entity id, const char* componentName, const void* data){
    size_t componentType = ecs->componentRegistry.at(componentName);

    push_back(&ecs->dense[componentType], data);
    ecs->sparse[componentType][id]= ecs->dense[componentType].count-1;
    ecs->denseToSparse[componentType].push_back(id);

};

Entity createEntity(Ecs* ecs){
    Entity id;
    if(ecs->removedEntities.size() > 0){
        id = ecs->removedEntities.back();
        ecs->removedEntities.pop_back();
    }else{
        id = ecs->entities;
        ecs->entities++;
    }
    //TODO: only for debug porpuse, so disable in release builds
    //DebugNameComponent debugName = {.name = name};
    //pushComponent(ecs, id, ECS_DEBUG_NAME, &debugName, sizeof(DebugNameComponent));
    return id;
}


bool hasComponentName(Ecs* ecs, const Entity entity, const char* componentName){
    size_t componentType = ecs->componentRegistry.at(componentName);
    if(ecs->sparse[componentType][entity] >= 0){
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

    for(Entity entity = 0; entity < ecs->entities; entity++){
        bool match = true;
        for(std::string componentName : names){
            if(!hasComponentName(ecs, entity, componentName.c_str())){
                match = false;
                break;
            }
        }
        if(match){
            entities.push_back(entity);
        }
    }
    va_end(args);
    return entities;

}

void* getComponentName(Ecs* ecs, Entity entity, const char* componentName){
    if(hasComponentName(ecs, entity, componentName)){
        //return ecs->dense[type][ecs->sparse[type][entity]].data;
        size_t componentType = ecs->componentRegistry.at(componentName);
        return get(&ecs->dense[componentType], ecs->sparse[componentType][entity]);
        //return ecs->dense[type][ecs->sparse[type][entity]];
    }else{
        return nullptr;
    }
}

void* getComponentVectorName(Ecs* ecs, const char* componentName){
    size_t componentType = ecs->componentRegistry.at(componentName);
    return ecs->dense[componentType].elements;
    //return ecs->dense[type].data();
}


void removeComponentName(Ecs* ecs, Entity entity, const char* componentName){
    if(hasComponentName(ecs, entity, componentName)){
        size_t componentType = ecs->componentRegistry.at(componentName);
        uint32_t denseIndex = ecs->sparse[componentType][entity];
        uint32_t denseLast = ecs->dense[componentType].count-1;
        if(denseLast < 0){
            return;
        }

        //if(ecs->denseToSparse[componentType].size() <= 0){
        //    return;
        //}
        uint32_t backEntity = ecs->denseToSparse[componentType][denseLast];

        if(denseIndex != denseLast){
            void* swapComp = get(&ecs->dense[componentType], denseLast);
            if(swapComp){
                insert(&ecs->dense[componentType], denseIndex, swapComp);
                ecs->sparse[componentType][backEntity] = denseIndex;
                ecs->denseToSparse[componentType][denseIndex] = backEntity;
            }else{
                return;
            }
        }
        pop_back(&ecs->dense[componentType]);
        //ecs->denseToSparse[componentType].erase(denseIndex);
        ecs->denseToSparse[componentType].pop_back();
        ecs->sparse[componentType][entity] = -1;
    }
}

void removeEntity(Ecs* ecs, Entity entity){
    for(auto& c : ecs->componentRegistry){
        //size_t componentType = ecs->componentRegistry[c.first.c_str()];
        removeComponentName(ecs, entity, c.first.c_str());
        //ecs->sparse[c.second][entity] = -1;
    }
    ecs->removedEntities.push_back(entity);
}

void clearEcs(Ecs* ecs){
    for(size_t entity = 0; entity < ecs->entities; entity++){
        if(hasComponent(ecs, entity, PersistentTag)){
            continue;
        }
        removeEntity(ecs, entity);
    }
    //for(size_t i = 0; i < ecs->componentRegistry.size(); i++){
    //    ecs->dense[i].count = 0;
    //    ecs->dense[i].elementSize = 0;
    //    free(ecs->dense[i].elements);
    //}
    //ecs->sparse.clear();
    //ecs->dense.clear();
    //ecs->denseToSparse.clear();
    //ecs->removedEntities.clear();
    //ecs->entities = 1;
    //ecs->componentId = 1;
}

void destroyEcs(Ecs* ecs){
    for(size_t entity = 0; entity < ecs->entities; entity++){
        removeEntity(ecs, entity);
    }
    for(size_t i = 0; i < ecs->componentRegistry.size(); i++){
        ecs->dense[i].count = 0;
        ecs->dense[i].elementSize = 0;
        free(ecs->dense[i].elements);
    }
    ecs->sparse.clear();
    ecs->dense.clear();
    ecs->denseToSparse.clear();
    ecs->removedEntities.clear();
    ecs->entities = 0;
    //ecs->componentId = 1;
    delete ecs;
}