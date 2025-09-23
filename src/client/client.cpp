#include <iostream>

#include <client.h>
#include <util.h>
#include <shared_test_class.h>

using namespace std;

int main() {
    SharedTestClass shared_test;
    cout << "Hello, client!\n" << TestClass::test() << '\n' << util::test_function() << '\n' << shared_test.test();
}