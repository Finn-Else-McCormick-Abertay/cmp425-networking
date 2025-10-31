#pragma once

#include <string>
#include <typeinfo>

class ITickingSystem {
public:
    ITickingSystem();
    virtual ~ITickingSystem();

    virtual void tick(float dt) = 0;
};