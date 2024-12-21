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

uint32_t createEntity(Ecs* ecs, const ComponentType type, const void* data, const size_t size){
    
    Entity id = ecs->entities;
    pushComponent(ecs, id, type, data, size);
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

    ecs->components[type].erase(id);

    //std::vector<Component>& component = ecs->components[type]; //Prendi la reference del vettore e non copiarlo 
    //for(int i = 0; i < component.size(); i++){
    //    if(component[i].id == id){
    //        LOGINFO("Component %d removed from entity %d", type, id);
    //        free(component[i].data);            //pulisco la memoria del puntatore void*
    //        component[i] = component.back();    //metto l'ultimo elemento del vettore nel componente che va rimosso
    //        component.pop_back();               //rimuovo l'ultimo componente dal vettore
    //    }
    //}
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
        LOGINFO("Entity %d removed", id);
        removeComponent(ecs, id, *it);
    }

    //remove every attached entity
    //for(int i = 1; i< ecs->entities; i++){
    //    componentTypes = ecs->entityComponentMap[i];
    //    for(auto it = componentTypes.begin(); it != componentTypes.end(); it++){
    //        if(ecs->components.find(it) != ecs->components.end()) {
    //            std::unordered_map<Entity, Component> entityComp = ecs->components.find(it);
    //            Component c = entityComp[i];
    //            AttachedEntity* a = (AttachedEntity*) c.data;
    //            if(a->entity){
    //                removeEntity(ecs, i);
    //            }
    //        }
    //    }
    //}
    ecs->entityComponentMap.erase(id);
    
}

std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents){
    PROFILER_START();
    std::vector<Entity> matchingEntities;
    //for (int i = 1; i <= ecs->entityComponentMap.size(); i++){
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