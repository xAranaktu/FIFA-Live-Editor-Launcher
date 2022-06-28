#include <config.h>

namespace core {
    Config::Config()
    {
    }

    Config::~Config()
    {
    }

    void Config::Setup(std::string folder) {
        logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

        fpath = folder + "\\launcher_config.json";

        if (!fs::exists(fpath)) {
            logger.Write(LOG_INFO, "[%s] Config not found: %s", __FUNCTION__, fpath.c_str());

            std::string msg = "Config not found:\n" + fpath;

            MessageBox(NULL, msg.c_str(), "WARNING", MB_ICONWARNING);
        }


    }

    void Config::Load() {
        logger.Write(LOG_INFO, "[%s]", __FUNCTION__);
        o = json::object();
        std::ifstream _stream(fpath);
        _stream >> o;

        logger.Write(LOG_INFO, "[%s] injection_delay", __FUNCTION__);
        if (!o.contains("injection_delay")) {
            o["injection_delay"] = injection_delay;
        }
        else {
            injection_delay = o.at("injection_delay").get<int>();
        }

        logger.Write(LOG_INFO, "[%s] auto_inject", __FUNCTION__);
        if (!o.contains("auto_inject")) {
            o["auto_inject"] = auto_inject;
        }
        else {
            auto_inject = o.at("auto_inject").get<bool>();
        }

        LoadFromStrArray(proc_names, "proc_names");
        LoadFromStrArray(dlls, "dlls");
    }

    void Config::Save() {
        logger.Write(LOG_INFO, "[%s]", __FUNCTION__);

        o.at("injection_delay") = injection_delay;
        o.at("auto_inject") = auto_inject;

        std::ofstream x(fpath.c_str());

        if (!x) {
            logger.Write(LOG_ERROR, "[%s] Can't write to: %s", __FUNCTION__, fpath.c_str());
            return;
        }

        x << std::setw(4) << o << std::endl;
        x.close();
    }

    void Config::LoadFromStrArray(std::vector<std::string>& to, std::string from) {
        logger.Write(LOG_INFO, "[%s] %s", __FUNCTION__, from.c_str());

        json array_o;
        bool is_array = false;
        bool contains = o.contains(from);

        if (contains) {
            array_o = o[from];
            is_array = array_o.is_array();
        }

        if (contains && is_array) {
            to.clear();
            for (auto it : array_o)
            {
                to.push_back(it);
            }
        }
        else {
            auto arr = json::array();

            for (auto s : to) {
                arr.push_back(s);
            }


            o[from] = arr;
        }
    }
}


core::Config g_Config;