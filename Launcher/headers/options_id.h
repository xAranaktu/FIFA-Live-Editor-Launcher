#pragma once
#pragma warning(disable: 4996)

#include <filesystem>
#include <fstream>
#include "localize.h"
#include <logger/logger.h>
#include "misc.h"
#include "../external/nlohmann/json.hpp"
#include "../external/nlohmann/fifo_map.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

class OptionsIds {
public:
    std::map<std::string, nlohmann::fifo_map<int, std::string>> options;

    void Clear();
    void LoadJson();

    bool HasOptions(std::string key);
    void AddOptions(std::string key, nlohmann::fifo_map<int, std::string> value);
    void AddOption(std::string key, int v, std::string name);
    nlohmann::fifo_map<int, std::string> GetOptions(std::string key, bool do_check = false);
private:
    std::filesystem::path file_path;
};

extern OptionsIds g_options_ids;