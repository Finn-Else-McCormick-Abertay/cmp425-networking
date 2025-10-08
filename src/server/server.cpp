#include <iostream>

#include <box2cpp/box2cpp.h>

#include <test.h>

using namespace std;

int main() {
    cout << "Server init" << endl;

    Test::foo();
    
    b2::World w(b2::World::Params{});
}