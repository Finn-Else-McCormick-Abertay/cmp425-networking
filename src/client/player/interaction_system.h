#pragma once

#include <system/system.h>
#include <terrain/world.h>

namespace player {
    class InteractionSystem : ITickingSystem {
    public:
        InteractionSystem(World* world);

        virtual void tick(float dt) override;
    
    private:
        World* _world;
    };

}