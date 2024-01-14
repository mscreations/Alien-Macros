#pragma once

#include <string>

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VERSION_MAJOR       1
#define VERSION_MINOR       0
#define VERSION_REVISION    0

// following is changed at build time by updateversion.ps1 to the most recent git hash. Only viewable on debug builds
#define VERSION_BUILD       208b687

#ifdef _DEBUG
#define VER_FILE_VERSION        VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR    STRINGIZE(VERSION_MAJOR) \
"." STRINGIZE(VERSION_MINOR) \
"." STRINGIZE(VERSION_REVISION) \
"." STRINGIZE(VERSION_BUILD)
#else
#define VER_FILE_VERSION        VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, 0
#define VER_FILE_VERSION_STR    STRINGIZE(VERSION_MAJOR) \
"." STRINGIZE(VERSION_MINOR) \
"." STRINGIZE(VERSION_REVISION)
#endif

#define VER_PRODUCT_VERSION     VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR VER_FILE_VERSION_STR

std::string GetAppVersion();
