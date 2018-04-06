#include <iostream>

#ifdef _NDEBUG
#define assert(condition) ((void)0)
#else
#define assert(condition) if (!condition) { std::cout << "Assertion `" #condition "` failed"; throw 1; }
#endif
