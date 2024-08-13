#include <config.h>

namespace core {
    void DirectoriesValues::to_json(json& j) {
        j = json{
            {"mods_root",               ToUTF8String(mods_root).c_str()},
            {"game_loc",                ToUTF8String(game_loc).c_str()},
            {"filters_storage",         ToUTF8String(filters_storage).c_str()},
            {"import_miniface",         ToUTF8String(import_miniface).c_str()},
            {"legacyfolder_export",     ToUTF8String(legacyfolder_export).c_str()},
            {"legacyfile_export",       ToUTF8String(legacyfile_export).c_str()},
            {"legacyfile_import",       ToUTF8String(legacyfile_import).c_str()}
        };
    }

    void DirectoriesValues::from_json(const json& j) {
        if (j.contains("mods_root")) {
            j.at("mods_root").get_to(mods_root);
        }

        if (j.contains("game_loc")) {
            j.at("game_loc").get_to(game_loc);
        }

        if (j.contains("filters_storage")) {
            j.at("filters_storage").get_to(filters_storage);
        }

        if (j.contains("import_miniface")) {
            j.at("import_miniface").get_to(import_miniface);
        }

        if (j.contains("legacyfolder_export")) {
            j.at("legacyfolder_export").get_to(legacyfolder_export);
        }

        if (j.contains("legacyfile_export")) {
            j.at("legacyfile_export").get_to(legacyfile_export);
        }

        if (j.contains("legacyfile_import")) {
            j.at("legacyfile_import").get_to(legacyfile_import);
        }

    }

    void OverlayValues::to_json(json& j) {
        j = json {
            {"stop_draw_at_startup",            stop_draw_at_startup},
            {"hide_all_windows_at_startup",     hide_all_windows_at_startup}
        };
    }

    void OverlayValues::from_json(const json& j) {
        if (j.contains("stop_draw_at_startup")) {
            j.at("stop_draw_at_startup").get_to(stop_draw_at_startup);
        }

        if (j.contains("hide_all_windows_at_startup")) {
            j.at("hide_all_windows_at_startup").get_to(hide_all_windows_at_startup);
        }
    }

    void UIValues::to_json(json& j) {
        j = json{
            {"scale",     scale},
        };
    }
    void UIValues::from_json(const json& j) {
        if (j.contains("scale")) {
            j.at("scale").get_to(scale);
        }
    }

    void LauncherValues::to_json(json& j) {
        j = json{
            {"show_disclaimer_msg",     show_disclaimer_msg},
            {"close_after_injection",   close_after_injection},
            {"auto_inject",             auto_inject},
            {"is_trial",                is_trial},
            {"injection_delay",         injection_delay},
            {"game_proc_name",          game_proc_name},
            {"game_proc_name_trial",    game_proc_name_trial},
            {"params",                  params},
            {"dlls",                    dlls}
        };
    }

    void LauncherValues::from_json(const json& j) {
        if (j.contains("show_disclaimer_msg")) {
            j.at("show_disclaimer_msg").get_to(show_disclaimer_msg);
        }

        if (j.contains("close_after_injection")) {
            j.at("close_after_injection").get_to(close_after_injection);
        }

        if (j.contains("auto_inject")) {
            j.at("auto_inject").get_to(auto_inject);
        }

        if (j.contains("is_trial")) {
            j.at("is_trial").get_to(is_trial);
        }

        if (j.contains("injection_delay")) {
            j.at("injection_delay").get_to(injection_delay);
        }

        if (j.contains("game_proc_name")) {
            j.at("game_proc_name").get_to(game_proc_name);
        }

        if (j.contains("game_proc_name_trial")) {
            j.at("game_proc_name_trial").get_to(game_proc_name_trial);
        }

        if (j.contains("params")) {
            j.at("params").get_to(params);
        }

        if (j.contains("dlls")) {
            j.at("dlls").get_to(dlls);
        }
    }

    void MatchFixingValues::to_json(json& j) {
        j = json{
            {"goals_scored",     goals_scored},
            {"goals_conceded",   goals_conceded}
        };
    }

    void MatchFixingValues::from_json(const json& j) {
        if (j.contains("goals_scored")) {
            j.at("goals_scored").get_to(goals_scored);
        }

        if (j.contains("goals_conceded")) {
            j.at("goals_conceded").get_to(goals_conceded);
        }
    }

    void HotkeysValues::Hotkey::to_json(json& j) {
        j = json{
            {"enabled",     enabled},
            {"keys_combination",   keys_combination}
        };
    }
    
    void HotkeysValues::Hotkey::from_json(const json& j) {
        if (j.contains("enabled")) {
            j.at("enabled").get_to(enabled);
        }

        if (j.contains("keys_combination")) {
            j.at("keys_combination").get_to(keys_combination);
        }
    }

    void HotkeysValues::to_json(json& j) {
        json obj_show_menu_keys = json::object();
        json obj_hide_ui_keys = json::object();

        show_menu_keys.to_json(obj_show_menu_keys);
        hide_ui_keys.to_json(obj_hide_ui_keys);

        j = json{
            {"show_menu_keys",  obj_show_menu_keys},
            {"hide_ui_keys",    obj_hide_ui_keys}
        };
    }

    void HotkeysValues::from_json(const json& j) {
        if (j.contains("show_menu_keys")) {
            show_menu_keys.from_json(j.at("show_menu_keys"));
        }

        if (j.contains("hide_ui_keys")) {
            hide_ui_keys.from_json(j.at("hide_ui_keys"));
        }
    }

    void LoggerValues::to_json(json& j) {
        j = json{
            {"log_game_logs",               log_game_logs},
            {"log_try_find_chunk_file",     log_try_find_chunk_file},
            {"log_iniread",                 log_iniread},
            {"log_level",                   log_level},
            {"log_load_chunk_file",         log_load_chunk_file},
            {"luaL_tolstring",              luaL_tolstring}
        };
    }

    void LoggerValues::from_json(const json& j) {
        if (j.contains("log_game_logs")) {
            j.at("log_game_logs").get_to(log_game_logs);
        }

        if (j.contains("log_try_find_chunk_file")) {
            j.at("log_try_find_chunk_file").get_to(log_try_find_chunk_file);
        }

        if (j.contains("log_iniread")) {
            j.at("log_iniread").get_to(log_iniread);
        }

        if (j.contains("log_level")) {
            j.at("log_level").get_to(log_level);
        }

        if (j.contains("log_load_chunk_file")) {
            j.at("log_load_chunk_file").get_to(log_load_chunk_file);
        }

        if (j.contains("luaL_tolstring")) {
            j.at("luaL_tolstring").get_to(luaL_tolstring);
        }
    }

    void OtherValues::to_json(json& j) {
        j = json{
            {"first_run",               first_run},
            // {"disable_chants",          disable_chants},
            {"show_player_potential",   show_player_potential},
            {"load_images",             load_images}
        };
    }

    void OtherValues::from_json(const json& j) {
        if (j.contains("first_run")) {
            j.at("first_run").get_to(first_run);
        }

        //if (j.contains("disable_chants")) {
        //    j.at("disable_chants").get_to(disable_chants);
        //}

        if (j.contains("show_player_potential")) {
            j.at("show_player_potential").get_to(show_player_potential);
        }

        if (j.contains("load_images")) {
            j.at("load_images").get_to(load_images);
        }
    }

    void DEBUGValues::to_json(json& j) {
        j = json{
            {"enable_crash_dump_gen",               enable_crash_dump_gen}
        };
    }

    void DEBUGValues::from_json(const json& j) {
        if (j.contains("enable_crash_dump_gen")) {
            j.at("enable_crash_dump_gen").get_to(enable_crash_dump_gen);
        }
    }

    Config::Config()
    {
    }

    Config::~Config()
    {
    }

    void Config::Load() {
        LOG_INFO(std::format("Load config from {}", ToUTF8String(fpath)));

        if (!fs::exists(fpath)) {
            LOG_WARN("Config File not found");
            return;
        }
        o.clear();
        std::ifstream _stream(fpath);
        try {
            o = json::parse(_stream);
            from_json(o);
        }
        catch (nlohmann::json::exception& e) {
            LOG_ERROR(std::format("Load Config Error {}", e.what()));
        }

        _stream.close();

        LOG_INFO(std::format("[{}] Done", __FUNCTION__));
    }

    void Config::Save() {
        LOG_INFO(std::format("[{}]", __FUNCTION__));

        try {
            to_json(o);
        }
        catch (nlohmann::json::exception& e) {
            LOG_ERROR(std::format("Save Config Error {}", e.what()));
            return;
        }
        

        std::ofstream x(fpath);

        if (!x) {
            LOG_ERROR(std::format("Can't Write To {}", ToUTF8String(fpath)));
            return;
        }

        x << std::setw(4) << o << std::endl;
        x.close();
    }

    void Config::Create() {
        LOG_INFO(std::format("[{}] {}", __FUNCTION__, ToUTF8String(fpath)));
        Save();
    }

    void Config::to_json(json& j) {
        json tmp = json::object();

        directories_values.to_json(tmp);
        o["Dirs"] = tmp;
        tmp.clear();

        overlay_values.to_json(tmp);
        o["Overlay"] = tmp;
        tmp.clear();

        ui_values.to_json(tmp);
        o["UI"] = tmp;
        tmp.clear();

        launch_values.to_json(tmp);
        o["Launcher"] = tmp;
        tmp.clear();

        matchfixing_values.to_json(tmp);
        o["Match-Fixing"] = tmp;
        tmp.clear();

        hotkeys_values.to_json(tmp);
        o["Hotkeys"] = tmp;
        tmp.clear();

        logger_values.to_json(tmp);
        o["Logger"] = tmp; 
        tmp.clear();

        other_values.to_json(tmp);
        o["Other"] = tmp;
        tmp.clear();

        debug_values.to_json(tmp);
        o["DEBUG"] = tmp;
        tmp.clear();
    }

    void Config::from_json(const json& j) {
        if (j.contains("Dirs")) {
            directories_values.from_json(j.at("Dirs"));
        }

        if (j.contains("Overlay")) {
            overlay_values.from_json(j.at("Overlay"));
        }

        if (j.contains("UI")) {
            ui_values.from_json(j.at("UI"));
        }

        if (j.contains("Launcher")) {
            launch_values.from_json(j.at("Launcher"));
        }

        if (j.contains("Match-Fixing")) {
            matchfixing_values.from_json(j.at("Match-Fixing"));
        }

        if (j.contains("Hotkeys")) {
            hotkeys_values.from_json(j.at("Hotkeys"));
        }

        if (j.contains("Logger")) {
            logger_values.from_json(j.at("Logger"));
        }

        if (j.contains("Other")) {
            other_values.from_json(j.at("Other"));
        }

        if (j.contains("DEBUG")) {
            debug_values.from_json(j.at("DEBUG"));
        }

    }
}


core::Config g_Config;