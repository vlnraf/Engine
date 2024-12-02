#include "ecs.hpp"
#include "tracelog.hpp"

Ecs* initEcs(){
    Ecs* ecs = new Ecs();
    ecs->entities = 0;
    return ecs;
}

void pushComponent(Ecs* ecs, Entity id, ComponentType type, void* data, size_t size){
    ecs->entityComponentMap[id].insert(type);
    //NOTE: Siccome ho una texture nel componente sto salvando piu volte la stessa texture ????
    Component c = {};
    c.id = id;
    void* copiedData = malloc(size);
    memcpy(copiedData, data, size);
    c.data = copiedData;

    ecs->components[type].push_back(c);
    //ecs->components.insert({type, data});
}

uint32_t createEntity(Ecs* ecs, ComponentType type, void* data, size_t size){
    Entity id = ecs->entities;
    pushComponent(ecs, id, type, data, size);
    ecs->entities++;
    return id;
}

void removeComponents(Ecs* ecs, Entity id, std::vector<ComponentType> types){
    for(int i = 0; i < types.size(); i++){
        removeComponent(ecs, id, types[i]);
    }
}

void removeComponent(Ecs* ecs, Entity id, ComponentType type){
    //TODO: Assume the index is always also the id of the entity
    //i am not sure it's always true
    //if not i have to scan all the array and match when id is equal to component[i].entityId
    //or a better approach would be to use an unordered_map<EntityId, Component> instead of a vector
    // to access the components in O(1) time
    //just let's try this way before optimize it

    std::vector<Component>& component = ecs->components[type]; //Prendi la reference del vettore e non copiarlo 
    for(int i = 0; i < component.size(); i++){
        if(component[i].id == id){
            free(component[i].data);            //pulisco la memoria del puntatore void*
            component[i] = component.back();    //metto l'ultimo elemento del vettore nel componente che va rimosso
            component.pop_back();               //rimuovo l'ultimo componente dal vettore
        }
    }
    ecs->entityComponentMap[id].erase(type);
}

void removeEntities(Ecs* ecs, std::vector<Entity> entities){
    for(int i = 0; i < entities.size(); i++){
        removeEntity(ecs, entities[i]);
    }
}

void removeEntity(Ecs* ecs, Entity id){
    std::unordered_set<ComponentType> componentTypes = ecs->entityComponentMap[id];
    for(auto it = componentTypes.begin(); it != componentTypes.end(); it++){
        removeComponent(ecs, id, *it);
    }
    ecs->entityComponentMap.erase(id);
}

std::vector<Entity> view(Ecs* ecs, const std::vector<ComponentType> requiredComponents){
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