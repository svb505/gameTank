#pragma once

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <format>

#pragma comment(lib, "psapi.lib")

class Profiler {
public:
    SIZE_T getRAM();
    SIZE_T getCPU();
    std::vector<std::string> getMemoryUsage();
};