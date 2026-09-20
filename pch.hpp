#pragma once

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <Windows.h>

#include <direct.h>
#include <Psapi.h>

// MSVC links the import library through this pragma, other compilers are told to
// link it by the build system instead and warn about the pragma being unknown.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma comment(lib, "psapi.lib")
#pragma GCC diagnostic pop
