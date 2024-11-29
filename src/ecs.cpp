#include "ecs.hpp"
#include "tracelog.hpp"

typedef void(*ComponentInitializer)(Ecs*, Entity);

ComponentInitializer componentInitializers[] = {
    initTransform,  // TRANSFORM
    initHealth,     // HEALTH
    initInput       // INPUT
};

Ecs* initEcs(uint32_t size){
    //Ecs* ecs = (Ecs*)malloc(sizeof(Ecs));
    Ecs* ecs = new Ecs();
    ecs->entities = 0;
    ecs->maxEntities = size;
    //memset(ecs->components.input, -1, 200);
    //memset(ecs->components.transforms, -1, 200);
    //memset(ecs->components.h, -1, 200);
    return ecs;
}

void initTransform(Ecs* ecs, Entity id){
    TransformComponent c = {};
    c.entityId = id;
    ecs->components.transforms.push_back(c);
    //ecs->components.transforms[id] = c;
    //ecs->components.transforms[id].entityId = id;
}

void initHealth(Ecs* ecs, Entity id){
    HealthComponent h = {};
    h.entityId = id;
    ecs->components.h.push_back(h);
    //ecs->components.transforms[id] = c;
    //ecs->components.h[id] = h;
    //ecs->components.h[id].entityId = id;
}

void initInput(Ecs* ecs, Entity id){
    InputComponent i = {};
    i.entityId = id;
    ecs->components.input.push_back(i);
    //ecs->components.transforms[id] = c;
    //ecs->components.input[id] = i;
    //ecs->components.input[id].entityId = id;
}

void pushComponent(Ecs* ecs, int id, ComponentType type){
    if(type < COMPONENT_TYPE_COUNT && componentInitializers[type]){
        componentInitializers[type](ecs, id);
    }else{
        LOGERROR("Unknown component type: %d", type);
    }
}

void createEntity(Ecs* ecs, std::vector<ComponentType> types){
    Entity entityId = ecs->entities;
    for(int i = 0; i < types.size(); i++){
        pushComponent(ecs, entityId, types[i]);
    }
    ecs->entities++;
}

void updateTranformers(Ecs* ecs, int id, glm::vec3 pos){
    ecs->components.transforms[id].position = pos;
}

void inputSystem(Ecs* ecs, InputComponent input){
    for(int i = 0; i < ecs->components.input.size(); i++){
        int id = ecs->components.input[i].entityId;
        if(id >= 0){
            updateTranformers(ecs, id, glm::vec3(input.x, input.y, 0.0f));
        }
    }
}

//int main(){
//    Ecs* ecs = initEcs(10);
//    //TransformComponent component;
//    //HealthComponent h;
//    InputComponent input;
//    std::vector<ComponentType> types = {TRANSFORM};
//    std::vector<ComponentType> types2 = {TRANSFORM, INPUT};
//    //ComponentType types[2] = {TRANSFORM, INPUT};
//    //ComponentType types1[1] = {TRANSFORM};
//    //ComponentType types2[2] = {TRANSFORM};
//    createEntity(ecs, types2);
//    createEntity(ecs, types2);
//    createEntity(ecs, types);
//    //pushComponent(ecs, 1, INPUT);
//    scanf("%f", &input.x);
//    inputSystem(ecs, input);
//    for(int i = 0; i < ecs->entities; i++){
//        LOGINFO("%f, %f, %f", ecs->components.transforms[i].position.x,
//                                ecs->components.transforms[i].position.y,
//                                ecs->components.transforms[i].position.z);
//    }
//    //input.x = 1;
//    //input.y = 1;
//    return 0;
//}