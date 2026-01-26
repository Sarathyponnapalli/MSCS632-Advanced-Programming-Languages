#include <iostream>
#include <string>
using namespace std;

int main()
{
    cout << "C++ Feature Analysis" << endl;

    // 1. Feature: Static Typing
    // The type is fixed at compilation. We cannot change it.
    int number = 50;
    cout << "[Static Type] Integer: " << number << endl;

    // The compiler strictly forbids this line:
    // number = "Hello"; // Error: invalid conversion
    // can only done using explicit casting, which is not type-safe.
    cout << "   -> C++ ensures type safety at compile-time." << endl;

    // 2. Feature: Strict Block Scope (Stack Unwinding)
    // Variables inside { } are destroyed immediately when the block ends.
    cout << "[Scope] Entering block..." << endl;
    {
        int blockVar = 999;
        cout << "   Inside block: " << blockVar << endl;
    } // blockVar is destroyed HERE.

    // This proves the variable is gone. The memory is freed.
    // cout << blockVar; // Error: 'blockVar' was not declared
    cout << "[Scope] Block exited. Variable destroyed instantly." << endl;

    return 0;
}