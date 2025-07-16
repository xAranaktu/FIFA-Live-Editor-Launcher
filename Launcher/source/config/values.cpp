#include <config/values.h>

namespace LE {
    void DirectoriesValues::to_json(json& j) {
        j = json{
            {"filters_storage",         ToUTF8String(filters_storage).c_str()},
            {"import_miniface",         ToUTF8String(import_miniface).c_str()},
            {"legacyfolder_export",     ToUTF8String(legacyfolder_export).c_str()},
            {"legacyfile_export",       ToUTF8String(legacyfile_export).c_str()},
            {"legacyfile_import",       ToUTF8String(legacyfile_import).c_str()}
        };
    }

    void DirectoriesValues::from_json(const json& j) {
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

    void OverlayValues::to_json(json& j) {
        j = json{
            {"show_overlay_at_startup",     show_overlay_at_startup},
        };
    }

    void OverlayValues::from_json(const json& j) {
        if (j.contains("show_overlay_at_startup")) {
            j.at("show_overlay_at_startup").get_to(show_overlay_at_startup);
        }
    }

    void LauncherValues::to_json(json& j) {
        j = json{
            {"show_disclaimer_msg",     show_disclaimer_msg},
            {"close_after_injection",   close_after_injection},
            {"auto_inject",             auto_inject},
            {"injection_delay",         injection_delay},
            {"game_proc_name",          game_proc_name},
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

        if (j.contains("injection_delay")) {
            j.at("injection_delay").get_to(injection_delay);
        }

        if (j.contains("game_proc_name")) {
            j.at("game_proc_name").get_to(game_proc_name);
        }

        if (j.contains("params")) {
            j.at("params").get_to(params);
        }

        if (j.contains("dlls")) {
            j.at("dlls").get_to(dlls);
        }
    }

    void HotkeysValues::to_json(json& j) {
        json obj_show_ui_keys = json::object();

        show_ui_keys.to_json(obj_show_ui_keys);

        j = json{
            {"show_ui_keys",    obj_show_ui_keys}
        };
    }

    void HotkeysValues::from_json(const json& j) {
        if (j.contains("show_ui_keys")) {
            show_ui_keys.from_json(j.at("show_ui_keys"));
        }
    }

    void MessageBoxesValues::to_json(json& j) {
        j = json{
            {"dont_show_messageboxes",     dont_show_messageboxes},
        };
    }
    void MessageBoxesValues::from_json(const json& j) {
        if (j.contains("dont_show_messageboxes")) {
            j.at("dont_show_messageboxes").get_to(dont_show_messageboxes);
        }
    }

    void LoggerValues::to_json(json& j) {
        j = json{
            {"log_level",                   log_level},
            {"log_try_find_legacy_file",    log_try_find_legacy_file},
            {"log_load_legacy_file",        log_load_legacy_file},
            {"log_career_mode_fcei",        log_career_mode_fcei},
            {"log_career_mode_text_bed_io", log_career_mode_text_bed_io},
            {"log_localize",                log_localize},
            {"log_events",                  log_events},
        };
    }

    void LoggerValues::from_json(const json& j) {
        if (j.contains("log_try_find_legacy_file")) {
            j.at("log_try_find_legacy_file").get_to(log_try_find_legacy_file);
        }

        if (j.contains("log_level")) {
            j.at("log_level").get_to(log_level);
        }

        if (j.contains("log_load_legacy_file")) {
            j.at("log_load_legacy_file").get_to(log_load_legacy_file);
        }

        if (j.contains("log_career_mode_text_bed_io")) {
            j.at("log_career_mode_text_bed_io").get_to(log_career_mode_text_bed_io);
        }

        if (j.contains("log_career_mode_fcei")) {
            j.at("log_career_mode_fcei").get_to(log_career_mode_fcei);
        }

        if (j.contains("log_events")) {
            j.at("log_events").get_to(log_events);
        }

        if (j.contains("log_localize")) {
            j.at("log_localize").get_to(log_localize);
        }
    }

    void TransfersValues::to_json(json& j) {
        j = json{
            {"auto_delete_presigned_contracts", auto_delete_presigned_contracts},
            {"auto_terminate_loans", auto_terminate_loans},
        };
    }

    void TransfersValues::from_json(const json& j) {
        if (j.contains("auto_delete_presigned_contracts")) {
            j.at("auto_delete_presigned_contracts").get_to(auto_delete_presigned_contracts);
        }

        if (j.contains("auto_terminate_loans")) {
            j.at("auto_terminate_loans").get_to(auto_terminate_loans);
        }
    }

    void OtherValues::to_json(json& j) {
        j = json{
            {"first_run",               first_run},
            {"show_player_potential",   show_player_potential},
            {"show_del_row_warning",    show_del_row_warning},
            {"auto_reload_images",      auto_reload_images}
        };
    }

    void OtherValues::from_json(const json& j) {
        if (j.contains("first_run")) {
            j.at("first_run").get_to(first_run);
        }

        if (j.contains("show_player_potential")) {
            j.at("show_player_potential").get_to(show_player_potential);
        }       

        if (j.contains("show_del_row_warning")) {
            j.at("show_del_row_warning").get_to(show_del_row_warning);
        }        
        
        if (j.contains("show_del_player_warning")) {
            j.at("show_del_player_warning").get_to(show_del_player_warning);
        }

        if (j.contains("auto_reload_images")) {
            j.at("auto_reload_images").get_to(auto_reload_images);
        }
    }

    void DEBUGValues::to_json(json& j) {
        j = json{
            {"crash_dump_gen",               crash_dump_gen}
        };
    }

    void DEBUGValues::from_json(const json& j) {
        if (j.contains("crash_dump_gen")) {
            j.at("crash_dump_gen").get_to(crash_dump_gen);
        }
    }
}