#include <Windows.h>
#include "MinHook.h"
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include "AOBScanner.h"
#include <mutex>          // std::mute
#include <thread>
#include <iostream>

#include <vector>
#include <map>

extern "C" void GetRax(INT64);
extern "C" INT64 SetRax(INT64);
extern "C" void GetAL(BYTE);

__declspec(dllexport) void SetupHooks();
