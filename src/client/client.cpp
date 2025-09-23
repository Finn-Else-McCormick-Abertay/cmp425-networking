#include <iostream>

#include <client.h>
#include <util.h>

using namespace std;

int main() {
    cout << "Hello, client!\n" << TestClass::test() << '\n' << util::test_function();
}