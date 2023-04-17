#pragma once
#include "options_id.h"


void OptionsIds::SetFile(std::filesystem::path folder) {
    std::filesystem::path fpath = folder / "data" / "id_map.json";
    logger.Write(LOG_INFO, "Set Dropdown Options File: %s", ToUTF8String(fpath).c_str());
    if (!fs::exists(fpath)) {
        logger.Write(LOG_ERROR, "File not found: %s", ToUTF8String(fpath).c_str());
        return;
    }

    file_path = fpath;
}
void OptionsIds::Clear() {

}

void OptionsIds::LoadJson() {
    logger.Write(LOG_INFO, "Set Dropdown Options from Json File: %s", ToUTF8String(file_path).c_str());

    json ids_map;
    try {
        std::ifstream f(file_path);
        ids_map = json::parse(f);
    }
    catch (nlohmann::json::exception& e) {
        logger.Write(LOG_WARN, "Options load error: %s", e.what());
        return;
    }

    for (auto& el : ids_map.items()) {
        auto k = el.key(); // "keys": {}
        auto entry = ids_map[k];

        bool translate = entry["translate"];
        for (auto& itms : entry["items"].items())
        {
            auto itm = itms.value();
            auto value = itm["value"].get<int>();
            auto name = itm["name"].get<std::string>();
            if (translate)
                name = localize.Translate(name.c_str());

            options[k][value] = name;
            //logger.Write(LOG_DEBUG, "%s itm name: %s, itm value: %d", k.c_str(), name.c_str(), value);
        }
    }
}


bool OptionsIds::HasOptions(std::string key) {
    return options.count(key) == 1;
}

void OptionsIds::AddOptions(std::string key, nlohmann::fifo_map<int, std::string> value) {
    if (HasOptions(key))    return;

    options[key] = value;
}

void OptionsIds::AddOption(std::string key, int v, std::string name) {
    logger.Write(LOG_INFO, "[%s] %s: %d = %s", __FUNCTION__, key.c_str(), v, name.c_str());

    options[key][v] = name;
}

nlohmann::fifo_map<int, std::string> OptionsIds::GetOptions(std::string key, bool do_check) {
    if (do_check) {
        if (!HasOptions(key)) {
            nlohmann::fifo_map<int, std::string> empty;
            return empty;
        }
    }
    return options.at(key);
}

OptionsIds g_options_ids;