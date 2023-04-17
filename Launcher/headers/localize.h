#pragma once
#include <string>
#include <map>
#include <filesystem>
#include "logger.h"
#include "misc.h"
#include "../external/nlohmann/json.hpp"

using json = nlohmann::json;

class Localize {
public:
    Localize();
    void SetLangPath(const std::filesystem::path& dllFolder);
    void LoadLangTrans(const std::string& lang);
    std::string Translate(const char* text, const std::map<std::string, std::string> data = {});
    std::string Translate(std::string text, const std::map<std::string, std::string> data = {});

private:
    std::string loadedLang = "";
    std::filesystem::path langPath = "";
    json j;
};

extern Localize localize;