#include <auth_manager/auth_manager.h>

namespace LE {
    AuthManager::AuthManager() {
        Clear();
    }

    AuthManager::~AuthManager() {}

    void AuthManager::Init() {
        Clear();
        user_agent = std::format("FC Live Editor {}", LE::VersionManager::GetInstance()->GetToolVersion());
        session_id = "session_" + std::to_string(GetTickCount64());
    }

    void AuthManager::SilentLogin() {
        if (IsFree()) {
            login_status = LOGIN_STATUS::HAS_ACCESS;
            Clear();

            fs::path auth_path = LE::FilesManager::GetInstance()->GetAuthPath();
            if (fs::exists(auth_path)) fs::remove(auth_path);

            return;
        }

        fs::path auth_path = LE::FilesManager::GetInstance()->GetAuthPath();

        // First Run
        if (!fs::exists(auth_path)) return;

        Load();

        // No access token
        if (access_token.empty()) return;

        GetDetails();
    }

    bool AuthManager::IsFree() {
        time_t now;
        time(&now);

        // Free Tier Got Access
        if (now >= UNLOCK_FREE) return true;

        return false;
    }

    void AuthManager::DoLogin() {
        LOG_FUNC_START();

        login_status = LOGIN_STATUS::NOT_LOGGED_IN;

        if (access_token.empty()) {
            LOG_INFO("[AuthManager] No access token, need to login");

            std::string client_id = "_0QudsO-zC8Xc6umgnyuhYQ7DRYLHSpAEMLlEh6TCVfF7igzpVgGsbpEshr6SxTD";
            std::string redirect_uri = std::format("{}/api/callback", server_url);
            std::string scope = "identity";

            std::string url = std::format(
                "https://www.patreon.com/oauth2/authorize?response_type=code&client_id={}&redirect_uri={}&scope={}&state={}",
                client_id, redirect_uri, scope, session_id
            );
            LOG_INFO(std::format("[AuthManager] Open URL: {}", url.c_str()));
            ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);

            std::thread(&AuthManager::PollForToken, this).detach();
        }
        else {
            GetDetails();
        } 
    }

    void AuthManager::DoLogout() {
        login_status = LOGIN_STATUS::NOT_LOGGED_IN;
        Clear();
        Save();
    }

    bool AuthManager::TierGotAccess(int tierid) {
        LOG_INFO(std::format("[{}] Check Tier: {}", __FUNCTION__, tierid));

        time_t now;
        time(&now);

        // Free Tier Got Access
        if (now >= UNLOCK_FREE) return true;

        const std::vector<int> allowedTiers = {
            4008263, // Silver
            3506908, // Gold
            2925473, // Hero
            3506912  // Legend
        };

        for (const auto& allowedTier : allowedTiers) {
            if (tierid == allowedTier) return true;
        }

        // Bronze Tier Got Access
        if (tierid == 3506900 && now >= UNLOCK_BRONZE) return true;

        return false;
    }

    bool AuthManager::VerifyAccess(const json& data) {
        LOG_FUNC_START();
        if (!data.contains("included") || !data["included"].is_array()) {
            LOG_ERROR("No included data found in response.");
            return false;
        }

        if (!data.contains("data")) {
            LOG_ERROR("No data found in response.");
            return false;
        }

        std::string note;
        patreon_userid = std::stoi(data["data"].value("id", "0"));
        LOG_INFO(std::format("USERID: {}", patreon_userid));

        for (const auto& item : data["included"]) {
            if (item.value("type", "") == "member") {
                std::string patron_status = item["attributes"].value("patron_status", "");

                // If they are actively paying
                if (patron_status == "active_patron") {
                    // Extract all the Tier IDs this membership entitles them to
                    if (item.contains("relationships") &&
                        item["relationships"].contains("currently_entitled_tiers") &&
                        item["relationships"]["currently_entitled_tiers"].contains("data")) {

                        for (const auto& tierData : item["relationships"]["currently_entitled_tiers"]["data"]) {
                            int tier_id = std::stoi(tierData.value("id", "0"));
                            if (TierGotAccess(tier_id)) {
                                LOG_INFO(std::format("[{}] User {} has access to tier ID: {}", __FUNCTION__, patreon_userid, tier_id));
                                return true;
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    void AuthManager::GetDetails() {
        login_status = LOGIN_STATUS::LOGIN_ATTEMPTED;

        std::string user_fields = "fields%5Buser%5D=about,created,email,full_name";
        std::string member_fields = "fields%5Bmember%5D=full_name,last_charge_date,last_charge_status,note,patron_status";
        std::string tier_fields = "fields%5Btier%5D=title";
        std::string full_url = std::format("https://www.patreon.com/api/oauth2/v2/identity?include=memberships,memberships.currently_entitled_tiers&{}&{}&{}", user_fields, member_fields, tier_fields);

        cpr::Response r = cpr::Get(
            cpr::Url{ full_url },
            cpr::Header{ {"Authorization", "Bearer " + access_token} }
        );

        if (r.status_code == 200) {
            json j = json::parse(r.text);
            LOG_INFO(r.text.c_str());
            if (VerifyAccess(j)) {
                LOG_INFO(std::format("[{}] User {} has access", __FUNCTION__, patreon_userid));
                Save();
                login_status = LOGIN_STATUS::HAS_ACCESS;
            }
            else {
                LOG_INFO(std::format("[{}] User {} does not have access", __FUNCTION__, patreon_userid));
                login_status = LOGIN_STATUS::NO_ACCESS;
                Clear();
                Save();
            }
        }
        else if (r.status_code == 401) {
            LOG_WARN(std::format(
                "[{}] Unauthorized. Access token may have expired. Clearing auth data. status code: {}. Error: {}",
                __FUNCTION__, r.status_code, r.error.message.c_str()
            ));
            Clear();
            Save();

            login_status = LOGIN_STATUS::TOKEN_EXPIRED;
        }
        else {
            LOG_ERROR(std::format(
                "[{}] Can't get response from: {}. status code: {}. Error: {}",
                __FUNCTION__, r.url.c_str(), r.status_code, r.error.message.c_str()
            ));
            Clear();
            Save();

            login_status = LOGIN_STATUS::UNKNOWN_ERROR;
        }
    }

    void AuthManager::PollForToken() {
        std::string request_url = std::format("{}/api/poll?session_id={}", server_url, session_id);
        LOG_INFO(std::format("[{}] Polling for token: {}", __FUNCTION__, request_url.c_str()));

        int max_attempts = 90; // Timeout after 3 minutes (90 attempts * 2 sec)
        int attempts = 0;

        while (attempts < max_attempts) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            attempts++;

            cpr::Response r = cpr::Get(
                cpr::Url{ request_url },
                cpr::Header{{ "User-Agent", user_agent }}
            );

            if (r.status_code == 200) {
                json j = json::parse(r.text);
                std::string status = j.value("status", "");
                std::string reason = j.value("reason", "");

                if (status == "completed") {
                    attempts = max_attempts + 1; // Exit loop
                    access_token = j.value("access_token", "");
                    patreon_userid = j.value("patreon_id", 0);

                    LOG_INFO(std::format("USERID: {}", patreon_userid));

                    login_status = LOGIN_STATUS::HAS_ACCESS;
                    Save();
                    return;
                }
                else if (status == "rejected") {
                    LOG_ERROR(std::format("[{}] Polling failed. status: {}, reason: {}", __FUNCTION__, status.c_str(), reason.c_str()));
                    break; // Exit loop
                }
            }
            else {
                LOG_ERROR(std::format(
                    "[{}] Error during polling. status code: {}. Error: {}",
                    __FUNCTION__, r.status_code, r.error.message.c_str()
                ));

                break; // Exit loop
            }
        }

        login_status = LOGIN_STATUS::NO_ACCESS;
        Clear();
        Save();
    }

    void AuthManager::Save() {
        fs::path auth_path = LE::FilesManager::GetInstance()->GetAuthPath();

        LOG_INFO(std::format("Save Auth To {}", ToUTF8String(auth_path).c_str()));

        json jauth = json::object();
        try {
            to_json(jauth);
        }
        catch (nlohmann::json::exception& e) {
            LOG_ERROR(std::format("Save Auth Error {}", e.what()));
            return;
        }

        std::ofstream _stream(auth_path);

        if (!_stream) {
            LOG_ERROR(std::format("Can't Write To {}", ToUTF8String(auth_path).c_str()));
            return;
        }

        _stream << std::setw(4) << jauth << std::endl;
        _stream.close();

        LOG_INFO("Auth Save Done");
    }
    
    void AuthManager::Load() {
        fs::path auth_path = LE::FilesManager::GetInstance()->GetAuthPath();
        LOG_INFO(std::format("Load Auth From {}", ToUTF8String(auth_path).c_str()));

        if (!fs::exists(auth_path)) {
            LOG_WARN("Auth file not found");
            return;
        }

        json jauth = json::object();
        std::ifstream _stream(auth_path);
        try {
            jauth = json::parse(_stream);
            from_json(jauth);
        }
        catch (nlohmann::json::exception& e) {
            LOG_ERROR(std::format("Load Auth Error {}", e.what()));
        }
        _stream.close();

        LOG_INFO("Auth Load Done");
    }

    void AuthManager::to_json(json& j) {
        j = json{
            {"access_token",    access_token},
            {"userid",          patreon_userid},
        };
    }
    
    void AuthManager::from_json(const json& j) {
        if (j.contains("access_token")) {
            j.at("access_token").get_to(access_token);
        }

        if (j.contains("userid")) {
            j.at("userid").get_to(patreon_userid);
        }
    }

    void AuthManager::Clear() {
        access_token.clear();
        patreon_userid = 0;
    }

    AuthManager* AuthManager::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new AuthManager();

        return pinstance_;
    }
}

LE::AuthManager* LE::AuthManager::pinstance_{ nullptr };
std::mutex LE::AuthManager::mutex_;