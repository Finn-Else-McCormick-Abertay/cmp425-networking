#include <iostream>

//#include <box2cpp/box2cpp.h>

#include <terrain/world.h>

using namespace std;

int main() {
    cout << "Server init" << endl;
    
    //b2::World w(b2::World::Params{});

    auto world = World();
}