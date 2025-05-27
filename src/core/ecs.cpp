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

//std::vector<Entity> view(Ecs* ecs, std::vector<ComponentType> types){
//    std::vector<Entity> entities; for(Entity entity = 0; entity < ecs->entities; entity++){
//        bool match = false;
//        for(ComponentType type : types){
//            if(hasComponent(ecs, entity, type)){
//                match = true;
//                continue;
//            }else{
//                match = false;
//            }
//        }
//        if(match){
//            entities.push_back(entity);
//        }
//    }
//    return entities;
//}


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

//void* getComponent(Ecs* ecs, Entity entity, ComponentType type){
//    if(hasComponent(ecs, entity, type)){
//        //return ecs->dense[type][ecs->sparse[type][entity]].data;
//        return get(&ecs->dense[type], ecs->sparse[type][entity]);
//        //return ecs->dense[type][ecs->sparse[type][entity]];
//    }else{
//        return nullptr;
//    }
//}


void* getComponentVectorName(Ecs* ecs, const char* componentName){
    size_t componentType = ecs->componentRegistry.at(componentName);
    return ecs->dense[componentType].elements;
    //return ecs->dense[type].data();
}

//void* getComponentVector(Ecs* ecs, ComponentType type){
//    return ecs->dense[type].elements;
//    //return ecs->dense[type].data();
//}


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



//void removeComponent(Ecs* ecs, Entity entity, ComponentType type){
//    if(hasComponent(ecs, entity, type)){
//        //Entity backIndex = ecs->dense[type].size()-1;
//        uint32_t denseIndex = ecs->sparse[type][entity];
//        //uint32_t backEntity = ecs->dense[type].back().entity;
//        uint32_t backEntity = ecs->denseToSparse[type].back();
//
//        //free(ecs->dense[type][denseIndex].data);
//        if(denseIndex != backEntity){
//            //ecs->dense[type][denseIndex] = ecs->dense[type].back();
//            //ecs->dense[type][denseIndex] = back(ecs->dense[type]);
//            insert(&ecs->dense[type], denseIndex, back(&ecs->dense[type]));
//            //Entity swappedEntity = ecs->dense[type][denseIndex].entity;
//            //Entity swappedEntity = ecs->denseToSparse[type][denseIndex];
//            ecs->sparse[type][backEntity] = denseIndex;
//            //ecs->sparse[type][backEntity] = denseIndex;
//        }
//        //ecs->dense[type].pop_back();
//        pop_back(&ecs->dense[type]);
//        ecs->denseToSparse[type].pop_back();
//        ecs->sparse[type][entity] = -1;
//    }
//}

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
        removeEntity(ecs, entity);
    }
    //for(size_t i = 0; i < ecs->componentRegistry.size(); i++){
    //    ecs->dense[i].count = 0;
    //    ecs->dense[i].elementSize = 0;
    //    free(ecs->dense[i].elements);
    //}
    //ecs->sparse.clear();
    //ecs->dense.clear();
    ecs->denseToSparse.clear();
    ecs->removedEntities.clear();
    ecs->entities = 0;
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


//std::vector<std::vector<Component>> viewComponents(Ecs* ecs, std::vector<ComponentType> types){
//    //std::unordered_map<ComponentType, std::vector<void*>> result;
//
//    std::vector<std::vector<Component>> result;
//    std::vector<Entity> entities;
//    for(Entity entity = 0; entity < ecs->entities; entity++){
//        bool match = false;
//        for(ComponentType type : types){
//            if(hasComponent(ecs, entity, type)){
//                match = true;
//                //break;
//                continue;
//            }else{
//                match = false;
//            }
//        }
//        if(match){
//            entities.push_back(entity);
//        }
//    }
//    //NOTE: i componenti vengono restituiti in base al vettore di query
//    for(int i = 0; i < types.size(); i++){
//        //std::vector<void*> r;
//        std::vector<Component> res;
//        for(Entity entity : entities){
//            if(getComponent(ecs, entity, types[i])){
//                res.push_back(ecs->dense[types[i]][ecs->sparse[types[i]][entity]]);
//            }
//        }
//        result.push_back(res);
//    }
//    return result;
//}
//Entity createEntity(Ecs* ecs, const std::string name, const ComponentType type, const void* data, const size_t size){
//    
//    Entity id = ecs->entities;
//    pushComponent(ecs, id, type, data, size);
//    //TODO: only for debug porpuse, so disable in release builds
//    DebugNameComponent debugName = {.name = name};
//    pushComponent(ecs, id, ECS_DEBUG_NAME, &debugName, sizeof(DebugNameComponent));
//    ecs->entities++;
//    
//    return id;
//}

//void removeComponents(Ecs* ecs, const Entity id, const std::vector<ComponentType> types){
//    for(int i = 0; i < types.size(); i++){
//        removeComponent(ecs, id, types[i]);
//    }
//}
//
//void removeComponent(Ecs* ecs, const Entity id, const ComponentType type){
//    //TODO: Assume the index is always also the id of the entity
//    //i am not sure it's always true
//    //if not i have to scan all the array and match when id is equal to component[i].entityId
//    //or a better approach would be to use an unordered_map<EntityId, Component> instead of a vector
//    // to access the components in O(1) time
//    //just let's try this way before optimize it
//
//    
//    if(id >= ecs->entities){
//        LOGERROR("Invalid entity ID: %d", id);
//        return;
//    }
//    // Find the component for the given type and entity
//    auto it = ecs->components[type].find(id);
//    if (it != ecs->components[type].end()) {
//        // Free the allocated memory for the component's data
//        free(it->second.data);
//        // Erase the component from the map
//        ecs->components[type].erase(it);
//    }
//    ecs->entityComponentMap[id].erase(type);
//}
//
//void removeEntities(Ecs* ecs, const std::vector<Entity> entities){
//    
//    for(int i = 0; i < entities.size(); i++){
//        removeEntity(ecs, entities[i]);
//    }
//    
//}
//
//void removeEntity(Ecs* ecs, const Entity id){
//    
//    if(id >= ecs->entities){
//        LOGERROR("Invalid entity ID: %d", id);
//        return;
//    }
//
//    std::unordered_set<ComponentType> componentTypes = ecs->entityComponentMap[id];
//    for(auto it = componentTypes.begin(); it != componentTypes.end(); it++){
//        //LOGINFO("Componet %d removed from entity %d", *it, id);
//        removeComponent(ecs, id, *it);
//    }
//
//    ecs->entityComponentMap.erase(id);
//    //LOGINFO("Entity %d removed", id);
//}
//
//std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents){
//    PROFILER_START();
//    std::vector<Entity> matchingEntities;
//    bool match = false;
//    for(ComponentType type : requiredComponents){
//        for(uint32_t compInd : ecs->sparse[type]){
//        }
//    }
//    //for(const auto& entity : ecs->entityComponentMap){
//    //    //Entity entity = i;
//    //    std::unordered_set<ComponentType>& components = ecs->entityComponentMap[entity.first];
//    //    bool matches = true;
//    //    for (int j = 0; j < requiredComponents.size(); j++) {
//    //        if (components.find(requiredComponents[j]) == components.end()) {
//    //            matches = false;
//    //            break;
//    //        }
//    //    }
//    //    if (matches) {
//    //        matchingEntities.push_back(entity.first);
//    //    }
//    //}
//
//    PROFILER_END();
//    
//    return matchingEntities;
//}
//
//
//void* getComponent(Ecs* ecs, const Entity id, const ComponentType type){
//    PROFILER_START();
//    if(id >= ecs->entities){
//        LOGERROR("Invalid entity ID: %d", id);
//        return nullptr;
//    }
//
//    if(ecs->components[type].find(id) == ecs->components[type].end()) {
//        LOGERROR("entity %d has no component %d", id, type);
//        return nullptr;
//    }
//
//    Component c = ecs->components[type].at(id);
//    PROFILER_END();
//    return c.data;
//}
//
//void ecsDestroy(Ecs* ecs){
//    //TODO
//    //for(const auto& pair : ecs->entityComponentMap){
//    //    removeEntity(ecs, pair.first);
//    //}
//    for (const auto& entity : ecs->entityComponentMap) {
//        removeEntity(ecs, entity.first);  // Frees all components associated with entities
//    }
//
//    // Clear the components map (in case anything remains)
//    for (auto& [type, components] : ecs->components) {
//        for (auto& [id, component] : components) {
//            free(component.data);  // Free dynamically allocated data
//        }
//        components.clear();
//    }
//    ecs->components.clear();
//
//    //free(ecs);
//    delete ecs;
//}

//TODO: manage the memory when you set the new component, you have to free the previos one
//void setComponent(Ecs* ecs, const Entity id, void* data, const ComponentType type){
//    PROFILER_START();
//    if(id >= ecs->entities){
//        LOGERROR("Invalid entity ID: %d", id);
//        return;
//    }
//    if(ecs->components[type].find(id) == ecs->components[type].end()){
//        LOGERROR("entity ID: %d has no component %d", id, type);
//        return;
//    }
//
//    ecs->components[type].at(id).data = data;
//}