#pragma once
#include <string>
#include <map>
#include <filesystem>
#include <logger/logger.h>
#include <files_manager/files_manager.h>
#include "misc.h"
#include "../external/nlohmann/json.hpp"

using json = nlohmann::json;

class Localize {
public:
    Localize();
    ~Localize();

    void from_json(const json& j);

    void Load();

    std::string Translate(const char* text, const std::map<std::string, std::string> data = {});
    std::string Translate(std::string text, const std::map<std::string, std::string> data = {});
private:
    std::map<std::string, std::string> translated_strings;
};

extern Localize localize;