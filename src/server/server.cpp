#include <iostream>

//#include <box2cpp/box2cpp.h>

#include <terrain/world.h>
#include <util/console.h>

using namespace std;

int main() {
    console::info("Server init");
    
    //b2::World w(b2::World::Params{});

    auto world = World();
}