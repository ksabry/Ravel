#pragma once

#include <iostream>

#ifdef _NDEBUG
#define Assert(condition) ((void)0)
#else
#define Assert(condition) if (!(condition)) { std::cout << "Assertion `" #condition "` failed"; throw 1; }
#endif
