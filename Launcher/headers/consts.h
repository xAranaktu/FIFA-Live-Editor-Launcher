#pragma once
#include <map>
#include <vector>

inline const int FIFA_EDITION = 24;
inline const char* TOOL_NAME = "Live Editor";
inline const char* TOOL_VERSION = "v24.1.2.0";
inline const char DisclaimerContent[] = {
    "Using this put you in risk of getting banned. I don't know if you will still be able to access your game account after that happen.\nAlso, EA may patch this tool at any time and from that point it may be not possible to update it at all. Enjoy while you can"
};

// LE - Game Compatibility

// Game version - Title Update
// Game version from <GameInstallDir>/__Installer/installerdata.xml
inline const std::map<std::string, std::string> GAME_VERSION_TU_MAP = {
    { "1.0.83.56686",   "Vanilla"},
    { "1.0.84.4417",    "TU1"},
    { "1.0.84.7390",    "TU2"},
    { "1.0.84.10708",   "TU3"},
    { "1.0.84.28561",   "TU4"},
    { "1.0.84.34605",   "TU5"},
    { "1.0.84.58490",   "TU6"},
    { "1.0.85.25441",   "TU7"},
    { "1.0.85.33923",   "TU8"},
    { "1.0.85.48150",   "TU9"},
    { "1.0.86.18082",   "TU10"},
    { "1.0.86.43378",   "TU11"},
    { "1.0.86.54938",   "TU11.1"},
    { "1.0.87.7970",    "TU12"},
    { "1.0.87.30122",    "TU13"},
    { "1.0.87.55322",    "TU14"},
    { "1.0.88.42253",    "TU15"},
    { "1.0.89.6557",    "TU15.1"},
    { "1.0.89.16451",   "TU16"},
    { "1.0.89.28334",   "TU17"},
    { "1.0.90.3389",    "TU18"},
    { "1.0.90.31158",   "TU18.2"}
};

inline const std::vector<std::string> COMPATIBLE_TITLE_UPDATES = {
    "TU18.2"
};

inline const int LATEST_TU = 18;
inline const int LATEST_MAJOR_GAME_VER = 90;
inline const int LATEST_MINOR_GAME_VER = 31158;

