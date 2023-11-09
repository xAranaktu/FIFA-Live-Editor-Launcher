#pragma once
#include <map>
#include <vector>

inline const int FIFA_EDITION = 24;
inline const char* TOOL_NAME = "Live Editor";
inline const char* TOOL_VERSION = "v24.1.0.9";
inline const char DisclaimerContent[] = {
    "Using this put you in risk of getting banned. I don't know if you will still be able to access your game account after that happen.\nAlso, EA may patch this tool at any time and from that point it may be not possible to update it at all. Enjoy while you can"
};

// LE - Game Compatibility

// Game version - Title Update
// Game version from <GameInstallDir>/__Installer/installerdata.xml
inline const std::map<std::string, std::string> GAME_VERSION_TU_MAP = {
    { "1.0.83.56686",   "Vanilla"},
    { "1.0.84.7390",    "TU2"},
    { "1.0.84.10708",   "TU3"},
    { "1.0.84.28561",   "TU4"},
    { "1.0.84.34605",   "TU5"}
};

inline const std::vector<std::string> COMPATIBLE_TITLE_UPDATES = {
    "TU2", "TU3", "TU4", "TU5"
};

inline const int LATEST_TU = 5;
inline const int LATEST_MAJOR_GAME_VER = 84;
inline const int LATEST_MINOR_GAME_VER = 34605;

