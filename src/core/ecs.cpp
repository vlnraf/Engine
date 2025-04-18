#include "ecs.hpp"
#include "tracelog.hpp"
#include "profiler.hpp"

#include <string.h>

Ecs* initEcs(){
    Ecs* ecs = new Ecs();
    ecs->entities = 1;
    return ecs;
}

void pushComponent(Ecs* ecs, const Entity id, const ComponentType type, const void* data, const size_t size){
    
    ecs->entityComponentMap[id].insert(type);
    Component c = {};
    void* copiedData = malloc(size);
    memcpy(copiedData, data, size);
    c.data = copiedData;

    //ecs->components[type].push_back(c);
    ecs->components[type].insert({id, c});
}

Entity createEntity(Ecs* ecs){
    Entity id = ecs->entities;
    //TODO: only for debug porpuse, so disable in release builds
    //DebugNameComponent debugName = {.name = name};
    //pushComponent(ecs, id, ECS_DEBUG_NAME, &debugName, sizeof(DebugNameComponent));
    ecs->entities++;
    return id;
}

Entity createEntity(Ecs* ecs, const std::string name, const ComponentType type, const void* data, const size_t size){
    
    Entity id = ecs->entities;
    pushComponent(ecs, id, type, data, size);
    //TODO: only for debug porpuse, so disable in release builds
    DebugNameComponent debugName = {.name = name};
    pushComponent(ecs, id, ECS_DEBUG_NAME, &debugName, sizeof(DebugNameComponent));
    ecs->entities++;
    
    return id;
}

void removeComponents(Ecs* ecs, const Entity id, const std::vector<ComponentType> types){
    
    for(int i = 0; i < types.size(); i++){
        removeComponent(ecs, id, types[i]);
    }
    
}

void removeComponent(Ecs* ecs, const Entity id, const ComponentType type){
    //TODO: Assume the index is always also the id of the entity
    //i am not sure it's always true
    //if not i have to scan all the array and match when id is equal to component[i].entityId
    //or a better approach would be to use an unordered_map<EntityId, Component> instead of a vector
    // to access the components in O(1) time
    //just let's try this way before optimize it

    
    if(id >= ecs->entities){
        LOGERROR("Invalid entity ID: %d", id);
        return;
    }
    // Find the component for the given type and entity
    auto it = ecs->components[type].find(id);
    if (it != ecs->components[type].end()) {
        // Free the allocated memory for the component's data
        free(it->second.data);
        // Erase the component from the map
        ecs->components[type].erase(it);
    }
    ecs->entityComponentMap[id].erase(type);
}

void removeEntities(Ecs* ecs, const std::vector<Entity> entities){
    
    for(int i = 0; i < entities.size(); i++){
        removeEntity(ecs, entities[i]);
    }
    
}

void removeEntity(Ecs* ecs, const Entity id){
    
    if(id >= ecs->entities){
        LOGERROR("Invalid entity ID: %d", id);
        return;
    }

    std::unordered_set<ComponentType> componentTypes = ecs->entityComponentMap[id];
    for(auto it = componentTypes.begin(); it != componentTypes.end(); it++){
        LOGINFO("Componet %d removed from entity %d", *it, id);
        removeComponent(ecs, id, *it);
    }

    ecs->entityComponentMap.erase(id);
    LOGINFO("Entity %d removed", id);
}

std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents){
    PROFILER_START();
    std::vector<Entity> matchingEntities;
    for(const auto& entity : ecs->entityComponentMap){
        //Entity entity = i;
        std::unordered_set<ComponentType>& components = ecs->entityComponentMap[entity.first];
        bool matches = true;
        for (int j = 0; j < requiredComponents.size(); j++) {
            if (components.find(requiredComponents[j]) == components.end()) {
                matches = false;
                break;
            }
        }
        if (matches) {
            matchingEntities.push_back(entity.first);
        }
    }
    PROFILER_END();
    
    return matchingEntities;
}


void* getComponent(Ecs* ecs, const Entity id, const ComponentType type){
    PROFILER_START();
    if(id >= ecs->entities){
        LOGERROR("Invalid entity ID: %d", id);
        return nullptr;
    }

    if(ecs->components[type].find(id) == ecs->components[type].end()) {
        LOGERROR("No entity with component %d", type);
        return nullptr;
    }

    Component c = ecs->components[type].at(id);
    PROFILER_END();
    return c.data;
}

void ecsDestroy(Ecs* ecs){
    //TODO
    //for(const auto& pair : ecs->entityComponentMap){
    //    removeEntity(ecs, pair.first);
    //}
    for (const auto& entity : ecs->entityComponentMap) {
        removeEntity(ecs, entity.first);  // Frees all components associated with entities
    }

    // Clear the components map (in case anything remains)
    for (auto& [type, components] : ecs->components) {
        for (auto& [id, component] : components) {
            free(component.data);  // Free dynamically allocated data
        }
        components.clear();
    }
    ecs->components.clear();

    //free(ecs);
    delete ecs;
}

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