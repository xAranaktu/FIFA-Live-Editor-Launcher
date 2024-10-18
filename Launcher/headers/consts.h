#pragma once
#include <map>
#include <vector>

inline const int EAFC_EDITION = 25;
inline const char* TOOL_NAME = "Live Editor";
inline const char* TOOL_VERSION = "v25.1.7";
inline const char* GAME_WINDOW_CLS = "EA SPORTS FC 25";
inline const char DisclaimerContent[] = {
    "Using this put you in risk of getting banned. I don't know if you will still be able to access your game account after that happen.\nAlso, EA may patch this tool at any time and from that point it may be not possible to update it at all. Enjoy while you can"
};

// LE - Game Compatibility

// Game version - Title Update
// Game version from <GameInstallDir>/__Installer/installerdata.xml
inline const std::map<std::string, std::string> GAME_VERSION_TU_MAP = {
    { "1.0.116.27276",      "TU3"},
    { "1.0.116.10209",      "TU2"},
    { "1.0.116.295",        "TU1"},
    { "1.0.115.53355",      "Vanilla"}
};

inline const std::vector<std::string> COMPATIBLE_TITLE_UPDATES = {
    "TU2", "TU3"
};

inline const int LATEST_TU = 3;
inline const int LATEST_MAJOR_GAME_VER = 116;
inline const int LATEST_MINOR_GAME_VER = 27276;

