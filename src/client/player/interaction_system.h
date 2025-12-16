#pragma once

#include <system/system.h>
#include <world/level.h>

namespace player {
    class InteractionSystem : ITickingSystem {
    public:
        virtual void tick(float dt) override;
    };

}