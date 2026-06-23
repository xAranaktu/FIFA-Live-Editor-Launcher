#pragma once

#include <mutex>
#include <logger/logger.h>

namespace LE {
    class ServerManager {
    public:
        ServerManager(ServerManager& other) = delete;
        void operator=(const ServerManager&) = delete;
        static ServerManager* GetInstance();

        void SetLoggedIn(bool logged_in) { is_logged_in = logged_in; }
        std::string Test() { return "ServerManager TEST"; }

    private:
        static ServerManager* pinstance_;
        static std::mutex mutex_;

        bool is_logged_in = false;

    protected:
        ServerManager();
        ~ServerManager();
    };
}