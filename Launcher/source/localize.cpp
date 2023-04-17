#include "localize.h"

Localize::Localize() {
}


void Localize::SetLangPath(const std::filesystem::path& dllFolder) {
    langPath = dllFolder / "Lang";
    logger.Write(LOG_DEBUG, "SetLangPath: %s", ToUTF8String(langPath).c_str());
}

void Localize::LoadLangTrans(const std::string& lang) {
    logger.Write(LOG_INFO, "[%s] %s", __FUNCTION__, ToUTF8String(langPath).c_str());
    loadedLang = lang;
    const std::filesystem::path langFile = langPath / lang / "translate.json";

    if (!fs::exists(langFile)) {
        logger.Write(LOG_ERROR, "[%s] Can't Find %s", __FUNCTION__, ToUTF8String(langFile).c_str());
        return;
    }

    std::ifstream transtream(langFile);

    transtream >> j;
    logger.Write(LOG_INFO, "Language loaded");
}

std::string Localize::Translate(const char* text, const std::map<std::string, std::string> data) {
    std::string to_format;

    if (j.count(text) == 0) {
        // logger.Write(LOG_WARN, "Missing translation, lang: %s, text: %s", loadedLang.c_str(), text);
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
