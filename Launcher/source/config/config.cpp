#include <config/config.h>

namespace LE {
    Config::Config() {}
    Config::~Config() {}

    void Config::Init(fs::path path) {
        launch_values.game_proc_name = std::format("FC{}.exe", EAFC_EDITION);
        launch_values.game_proc_name_trial = std::format("FC{}_Trial.exe", EAFC_EDITION);
        launch_values.params.clear();   // No params by default

        cfg_path = path;

        // Create Config File on First Run
        fs::exists(cfg_path) ? Load() : Save();
    }

    void Config::Load() {
        LOG_INFO(std::format("Load Config From {}", ToUTF8String(cfg_path).c_str()));

        if (!fs::exists(cfg_path)) {
            LOG_WARN("Config file not found");
            return;
        }

        json cfg = json::object();
        std::ifstream _stream(cfg_path);
        try {
            cfg = json::parse(_stream);
            from_json(cfg);
        }
        catch (nlohmann::json::exception& e) {
            LOG_ERROR(std::format("Load Config Error {}", e.what()));
        }
        _stream.close();

        LOG_INFO("Config Load Done");
    }

    void Config::Save() {
        LOG_INFO(std::format("Save Config To {}", ToUTF8String(cfg_path).c_str()));

        json cfg = json::object();
        try {
            to_json(cfg);
        }
        catch (nlohmann::json::exception& e) {
            LOG_ERROR(std::format("Save Config Error {}", e.what()));
            return;
        }

        std::ofstream _stream(cfg_path);

        if (!_stream) {
            LOG_ERROR(std::format("Can't Write To {}", ToUTF8String(cfg_path).c_str()));
            return;
        }

        _stream << std::setw(4) << cfg << std::endl;
        _stream.close();

        LOG_INFO("Config Save Done");
    }

    void Config::to_json(json& j) {
        json tmp = json::object();

        ui_values.to_json(tmp);
        j["ui"] = tmp;
        tmp.clear();

        overlay_values.to_json(tmp);
        j["overlay"] = tmp;
        tmp.clear();

        launch_values.to_json(tmp);
        j["launcher"] = tmp;
        tmp.clear();

        hotkeys_values.to_json(tmp);
        j["hotkeys"] = tmp;
        tmp.clear();

        logger_values.to_json(tmp);
        j["logger"] = tmp;
        tmp.clear();

        messageboxes_values.to_json(tmp);
        j["message_boxes"] = tmp;
        tmp.clear();

        transfers_values.to_json(tmp);
        j["transfers_values"] = tmp;
        tmp.clear();

        other_values.to_json(tmp);
        j["other"] = tmp;
        tmp.clear();

        debug_values.to_json(tmp);
        j["DEBUG"] = tmp;
        tmp.clear();

        directories_values.to_json(tmp);
        j["directories"] = tmp;
        tmp.clear();
    }

    void Config::from_json(const json& j) {
        if (j.contains("ui")) {
            ui_values.from_json(j.at("ui"));
        }

        if (j.contains("overlay")) {
            overlay_values.from_json(j.at("overlay"));
        }

        if (j.contains("launcher")) {
            launch_values.from_json(j.at("launcher"));
        }

        if (j.contains("hotkeys")) {
            hotkeys_values.from_json(j.at("hotkeys"));
        }

        if (j.contains("logger")) {
            logger_values.from_json(j.at("logger"));
        }

        if (j.contains("message_boxes")) {
            messageboxes_values.from_json(j.at("message_boxes"));
        }

        if (j.contains("transfers_values")) {
            transfers_values.from_json(j.at("transfers_values"));
        }

        if (j.contains("other")) {
            other_values.from_json(j.at("other"));
        }

        if (j.contains("DEBUG")) {
            debug_values.from_json(j.at("DEBUG"));
        }

        if (j.contains("directories")) {
            directories_values.from_json(j.at("directories"));
        }
    }

    Config* Config::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new Config();

        return pinstance_;
    }
}

LE::Config* LE::Config::pinstance_{ nullptr };
std::mutex LE::Config::mutex_;