#include "lifetime.hpp"

ECS_DECLARE_COMPONENT(LifeTime)
void lifeTimeSystem(Ecs* ecs, float dt){
    EntityArray entities = view(ecs, ECS_TYPE(LifeTime));

    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        LifeTime* lifeTime = (LifeTime*) getComponent(ecs, e, LifeTime);
        if(lifeTime->time > lifeTime->endTime){
            removeEntity(ecs, e);
        }
        lifeTime->time += dt;
    }
}
