#include "ecs.hpp"
#include "tracelog.hpp"
#include "profiler.hpp"

#include <string.h>

Ecs* initEcs(){
    Ecs* ecs = new Ecs();
    ecs->entities = 0;

    for(int type = 0; type < ComponentType::COMPONENT_TYPE_COUNT; type++){
        for(int i = 0; i < MAX_ENTITIES; i++){
            ecs->sparse[(ComponentType)type].push_back(-1);
            //ecs->denseToSparse[(ComponentType)type].push_back(-1);
        }
    }

    return ecs;
}

void registerComponent(Ecs* ecs, ComponentType type, size_t size){
    //ecs->dense[type].reserve(MAX_COMPONENTS * size);
    //ecs->dense[type].reserve(size * MAX_COMPONENTS);
}

void pushComponent(Ecs* ecs, const Entity id, const ComponentType type, const void* data, const size_t size){
    
    Component c = {};
    void* copiedData = malloc(size);
    memcpy(copiedData, data, size);
    //c.entity = id;
    c.data = copiedData;
    ecs->dense[type].push_back(c);
    ecs->denseToSparse[type].push_back(id);
    ecs->sparse[type][id]= ecs->dense[type].size()-1;
}

Entity createEntity(Ecs* ecs){
    Entity id = ecs->entities;
    //TODO: only for debug porpuse, so disable in release builds
    //DebugNameComponent debugName = {.name = name};
    //pushComponent(ecs, id, ECS_DEBUG_NAME, &debugName, sizeof(DebugNameComponent));
    ecs->entities++;
    return id;
}

bool hasComponent(Ecs* ecs, const Entity entity, const ComponentType type){
    if(ecs->sparse[type][entity] != -1){
        return true;
    }else{
        return false;
    }
}

//Component getComponent2(Ecs* ecs, Entity entity, ComponentType type){
//    if(hasComponent(ecs, entity, type)){
//        return ecs->dense[type][entity];
//    }
//    return (Component){};
//}


std::vector<Entity> view(Ecs* ecs, std::vector<ComponentType> types){
    std::vector<Entity> entities;
    for(Entity entity = 0; entity < ecs->entities; entity++){
        bool match = false;
        for(ComponentType type : types){
            if(hasComponent(ecs, entity, type)){
                match = true;
                continue;
            }else{
                match = false;
            }
        }
        if(match){
            entities.push_back(entity);
        }
    }
    return entities;
}

void* getComponent(Ecs* ecs, Entity entity, ComponentType type){
    if(hasComponent(ecs, entity, type)){
        return ecs->dense[type][ecs->sparse[type][entity]].data;
        //return ecs->dense[type][ecs->sparse[type][entity]];
    }else{
        return nullptr;
    }
}

void* getComponentVector(Ecs* ecs, ComponentType type){
    return ecs->dense[type].data();
}

void removeComponent(Ecs* ecs, Entity entity, ComponentType type){
    if(hasComponent(ecs, entity, type)){
        //Entity backIndex = ecs->dense[type].size()-1;
        uint32_t denseIndex = ecs->sparse[type][entity];
        //uint32_t backEntity = ecs->dense[type].back().entity;
        uint32_t backEntity = ecs->denseToSparse[type].back();

        //free(ecs->dense[type][denseIndex].data);
        if(denseIndex != backEntity){
            ecs->dense[type][denseIndex] = ecs->dense[type].back();
            //Entity swappedEntity = ecs->dense[type][denseIndex].entity;
            Entity swappedEntity = ecs->denseToSparse[type][denseIndex];
            ecs->sparse[type][swappedEntity] = denseIndex;
            //ecs->sparse[type][backEntity] = denseIndex;
        }
        ecs->dense[type].pop_back();
        ecs->denseToSparse[type].pop_back();
        ecs->sparse[type][entity] = -1;
    }
}

void removeEntity(Ecs* ecs, Entity entity){
    for(int type = 0; type < ComponentType::COMPONENT_TYPE_COUNT; type++){
        if(hasComponent(ecs, entity, (ComponentType) type)){
            removeComponent(ecs, entity, (ComponentType) type);
            //ecs->sparse[(ComponentType) type][entity] = 0;
        }
    }
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