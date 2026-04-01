#pragma once

#include <system/system.h>
#include <data/namespaced_id.h>

namespace player {
    class InteractionSystem : ITickingSystem {
    public:
        virtual void tick(float dt) override;
    
    private:
        int _selected_hotbar_slot = 0;

        static id slot_to_tile_id(int slot);
    };

}