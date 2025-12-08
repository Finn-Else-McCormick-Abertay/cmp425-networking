#pragma once

#include <prelude.h>
#include <typeinfo>

class ITickingSystem {
public:
    ITickingSystem();
    virtual ~ITickingSystem();

    virtual void tick(float dt) = 0;
};

class IFixedTickingSystem {
public:
    IFixedTickingSystem();
    virtual ~IFixedTickingSystem();

    virtual void fixed_tick(uint64 elapsed_ticks) = 0;
};