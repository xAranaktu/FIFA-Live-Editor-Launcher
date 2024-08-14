#pragma once
#include "localize.h"

Localize::Localize() {}
Localize::~Localize() {}

void Localize::from_json(const json& j) {
    if (j.contains("translations")) {
        j.at("translations").get_to(translated_strings);
    }
}

void Localize::Load() {
    const std::filesystem::path langFile = LE::FilesManager::GetInstance()->GetLangPath();
    LOG_INFO(std::format("Load Language - {}", "eng_us"));

    if (!fs::exists(langFile)) {
        LOG_ERROR(std::format("Can't Find {}", ToUTF8String(langFile).c_str()));
        return;
    }

    json j = json::object();
    std::ifstream _stream(langFile);
    try {
        j = json::parse(_stream);
        from_json(j);
    }
    catch (nlohmann::json::exception& e) {
        LOG_ERROR(std::format("Language Load Error {}", e.what()));
    }
    _stream.close();

    LOG_INFO("Language Loaded");
}

std::string Localize::Translate(const char* text, const std::map<std::string, std::string> data) {
    std::string to_format;

    if (translated_strings.count(text) == 0) {
        translated_strings[text] = text;
    }

    to_format = translated_strings.at(text);

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
