#include "localize.h"

Localize::Localize() {
}


void Localize::SetLangPath(const std::filesystem::path& dllFolder) {
    langPath = dllFolder / "Lang";
}

void Localize::LoadLangTrans(const std::string& lang) {
    loadedLang = lang;
    const std::filesystem::path langFile = langPath / lang / "translate.json";
    LOG_INFO(std::format("Loading language: {}", ToUTF8String(langFile)));

    if (!fs::exists(langFile)) {
        LOG_ERROR(std::format("Can't Find {}", ToUTF8String(langFile)));
        return;
    }

    std::ifstream transtream(langFile);

    transtream >> j;
    LOG_INFO("Language loaded");
}

std::string Localize::Translate(const char* text, const std::map<std::string, std::string> data) {
    std::string to_format;

    if (j.count(text) == 0) {
        return std::string(text);
    }

    to_format = j.at(text);

    for (auto item : data) {
        std::string to_find = "{" + item.first + "}";
        std::size_t found = to_format.find(to_find);
        if (found != std::string::npos) {
            to_format.replace(found, to_find.length(), item.second);
        }
    }
    return to_format;
}

std::string Localize::Translate(std::string text, const std::map<std::string, std::string> data) {
    return Translate(text.c_str(), data);
}

Localize localize;
