#include "lifetime.hpp"
#include "componentIds.hpp"

void lifeTimeSystem(Ecs* ecs, float dt){
    EntityArray entities = view(ecs, (size_t[]){lifeTimeId}, 1);

    //for(Entity e : entities){
    for(size_t i = 0; i < entities.count; i++){
        Entity e = entities.entities[i];
        LifeTime* lifeTime = (LifeTime*) getComponent(ecs, e, lifeTimeId);
        if(lifeTime->time > lifeTime->endTime){
            removeEntity(ecs, e);
        }
        lifeTime->time += dt;
    }
}
