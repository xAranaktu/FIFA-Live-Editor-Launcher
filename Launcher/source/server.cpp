#include <server.h>


namespace LE {
    ServerManager::ServerManager() {}
    ServerManager::~ServerManager() {}

    ServerManager* ServerManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new ServerManager();

        return pinstance_;
    }
}

LE::ServerManager* LE::ServerManager::pinstance_{ nullptr };
std::mutex LE::ServerManager::mutex_;