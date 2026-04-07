#pragma once
#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <format>
#include "profiler.h"
#pragma comment(lib, "psapi.lib")

 SIZE_T Profiler::getRAM() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

SIZE_T Profiler::getCPU() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(),
        (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.PrivateUsage;
    }
    return 0;
}

std::vector<std::string> Profiler::getMemoryUsage() {
    return { std::format("RAM: {} MB",getRAM() / 1024 / 1024),
    std::format("CPU: {} MB",getCPU() / 1024 / 1024) };
}