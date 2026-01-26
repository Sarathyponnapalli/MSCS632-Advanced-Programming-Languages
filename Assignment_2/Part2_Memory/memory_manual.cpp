#include <iostream>
using namespace std;

void manualManagement()
{
    // 1. Allocate: Request 4 bytes (int) on the Heap using a pointer
    // \*ptr now holds the address of this memory
    int *ptr = new int(100);
    cout << "[Manual] Allocated integer at address: " << ptr << endl;
    cout << "[Manual] Value: " << *ptr << endl;

    // 2. Free: We MUST manually release this memory
    delete ptr;
    cout << "[Manual] Memory freed manually." << endl;
}

void memoryLeak()
{
    // 1. Allocate
    int *leakPtr = new int(500);
    cout << "[Leak] Allocated integer at address: " << leakPtr << endl;

    // ERROR: We explicitly 'forgot' to call delete.
    // This 4 bytes is now lost forever (until program ends).
    // A Profiler like Valgrind would flag this as "definitely lost".
}

int main()
{
    cout << "C++ Memory Management" << endl;
    manualManagement();

    cout << "Simulating Memory Leak" << endl;
    memoryLeak();

    return 0;
}