#include "lifetime.hpp"

void lifeTimeSystem(Ecs* ecs, float dt){
    auto entities = view(ecs, LifeTime);

    for(Entity e : entities){
        LifeTime* lifeTime = getComponent(ecs, e, LifeTime);
        if(lifeTime->time > lifeTime->endTime){
            removeEntity(ecs, e);
            //spikeTag->time= 0;
        }
        lifeTime->time += dt;
    }
}
