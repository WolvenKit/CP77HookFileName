#include <Windows.h>
#include "MinHook.h"

#include <string>
#include "psapi.h"
#include <process.h>
#include <string.h>
#include <vector>
#include <fstream>
#include "AOBScanner.h"
#include <thread>         // std::thread
#include <mutex>          // std::mute

extern "C" void GetRax(INT64);
extern "C" INT64 SetRax(INT64);
extern "C" void GetAL(BYTE);

__declspec(dllexport) void SetupHooks();
void UnHooks();