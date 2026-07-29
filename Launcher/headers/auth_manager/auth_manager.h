#pragma once

#include <mutex>
#include <logger/logger.h>
#include <../external/nlohmann/json.hpp>
#include <files_manager/files_manager.h>
#include <version_manager/version_manager.h>
#include <cpr/cpr.h>

using json = nlohmann::json;

inline const __int64 UNLOCK_FREE = 1982814891;
inline const __int64 UNLOCK_BRONZE = 1982814891;

namespace LE {
    enum class LOGIN_STATUS {
        NO_STATUS,
        NOT_LOGGED_IN,
        UNKNOWN_ERROR,
        LOGIN_ATTEMPTED,
        TOKEN_EXPIRED,
        NO_ACCESS,
        HAS_ACCESS
    };

    class AuthManager {
    public:
        AuthManager(AuthManager& other) = delete;
        void operator=(const AuthManager&) = delete;
        static AuthManager* GetInstance();

        void Init();
        void SilentLogin();
        void SetStatus(LOGIN_STATUS status) { login_status = status; }
        LOGIN_STATUS GetLoginStatus() const { return login_status; }
        bool IsFree();
        void DoLogin();
        void DoLogout();
        void GetDetails();
        bool VerifyAccess(const json& data);
        bool TierGotAccess(int tierid = 0);

        void PollForToken();

        int GetUserID() const { return patreon_userid; }

        void Save();
        void Load();

    private:
        static AuthManager* pinstance_;
        static std::mutex mutex_;

        std::string server_url = "https://fcle-auth.vercel.app";
        std::string user_agent = "";
        std::string session_id;
        std::string access_token;
        int patreon_userid = 0;
        
        LOGIN_STATUS login_status = LOGIN_STATUS::NOT_LOGGED_IN;

        void to_json(json& j);
        void from_json(const json& j);

        void Clear();

    protected:
        AuthManager();
        ~AuthManager();
    };
}