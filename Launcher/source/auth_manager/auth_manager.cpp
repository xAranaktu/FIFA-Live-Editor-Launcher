#include <auth_manager/auth_manager.h>

namespace LE {
    AuthManager::AuthManager() {
        Clear();
    }

    AuthManager::~AuthManager() {}

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
        // Bronze Tier Got Access
        // Free Tier Got Access
        if (now >= UNLOCK_FREE) return true;

        return false;
    }

    void AuthManager::DoLogin() {
        login_status = LOGIN_STATUS::NOT_LOGGED_IN;
        if (access_token.empty()) {
            LOG_INFO("[AuthManager] No access token, need to login");

            fs::path auth_server_path = fs::current_path() / "AuthServer" / "auth_server.exe";
            if (!fs::exists(auth_server_path)) {
                LOG_ERROR(std::format("[AuthManager] Auth Server not found: {}", ToUTF8String(auth_server_path).c_str()));
                MessageBoxA(NULL, "Auth Server not found", "ERROR", MB_ICONERROR);
                return;
            }

            LOG_INFO(std::format("[AuthManager] Starting Auth Server: {}", ToUTF8String(auth_server_path).c_str()));

            ShellExecuteA(NULL, "open", "AuthServer\\auth_server.exe", NULL, NULL, SW_HIDE);
            Sleep(1500); // Wait for the server to start

            std::string client_id = "_0QudsO-zC8Xc6umgnyuhYQ7DRYLHSpAEMLlEh6TCVfF7igzpVgGsbpEshr6SxTD";
            std::string redirect_uri = "http://localhost:2137/callback";
            std::string scope = "identity"; // Fill in your details

            std::string url = std::format(
                "https://www.patreon.com/oauth2/authorize?response_type=code&client_id={}&redirect_uri={}&scope={}", 
                client_id, redirect_uri, scope
            ); // Fill in your details
            LOG_INFO(std::format("[AuthManager] Open URL: {}", url.c_str()));
            ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
            return;
        }
        GetDetails();
    }

    bool AuthManager::TierGotAccess(std::string tier) {
        LOG_INFO(std::format("[{}] Check Tier: {}", __FUNCTION__, tier.c_str()));

        if (tier == "Silver" || tier == "Gold" || tier == "Hero" || tier == "Legend") {
            return true; 
        }

        time_t now;
        time(&now);
        // Bronze Tier Got Access
        if (tier == "Bronze" && now >= UNLOCK_BRONZE) return true;

        // Free Tier Got Access
        if (now >= UNLOCK_FREE) return true;

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

        std::vector<std::string> active_tier_ids;
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
                            active_tier_ids.push_back(tierData.value("id", ""));
                        }
                    }
                }
            }
        }

        if (active_tier_ids.empty()) {
            return false;
        }

        for (const auto& item : data["included"]) {
            if (item.value("type", "") == "tier") {
                std::string current_tier_id = item.value("id", "");

                // Check if this tier ID belongs to an active membership
                if (std::find(active_tier_ids.begin(), active_tier_ids.end(), current_tier_id) != active_tier_ids.end()) {
                    if (TierGotAccess(item["attributes"].value("title", ""))) {
                        login_status = LOGIN_STATUS::HAS_ACCESS;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void AuthManager::GetDetails() {
        login_status = LOGIN_STATUS::LOGIN_ATTEMPTED;
        LOG_INFO(std::format("[{}] access_token: {}, expires_in: {}", __FUNCTION__, access_token.c_str(), expires_in));

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

            return;
        }
        else {
            LOG_ERROR(std::format(
                "[{}] Can't get response from: {}. status code: {}. Error: {}",
                __FUNCTION__, r.url.c_str(), r.status_code, r.error.message.c_str()
            ));
            Clear();
            Save();

            login_status = LOGIN_STATUS::UNKNOWN_ERROR;

            return;
        }
        LOG_INFO(std::format("[{}] Got response from: {} in {:.2f}s", __FUNCTION__, r.url.c_str(), r.elapsed));
    }

    void AuthManager::DoAuth(std::string code) {
        LOG_INFO(std::format("[{}] code: {}", __FUNCTION__, code.c_str()));

        json j_body = { {"patreon_code", code} };
        cpr::Response r = cpr::Post(
            cpr::Url{ "https://fc-live-editor-patreon-backend.vercel.app/api/auth" },
            cpr::Header{ 
                {"User-Agent", "FC Live Editor Desktop Launcher"},
                {"Content-Type", "application/json"}
            },
            cpr::Body{ j_body.dump() }
        );
        if (r.status_code == 200) {
            json j = json::parse(r.text);
            LOG_INFO(r.text.c_str());

            if (j.contains("access_token")) {
                j.at("access_token").get_to(access_token);
            }

            if (j.contains("expires_in")) {
                j.at("expires_in").get_to(expires_in);
            }

            Save();
        }
        else {
            LOG_ERROR(std::format(
                "[{}] Can't get response from: {}. status code: {}. Error: {}",
                __FUNCTION__, r.url.c_str(), r.status_code, r.error.message.c_str()
            ));
            return;
        }

        LOG_INFO(std::format("[{}] Got response from: {} in {:.2f}s", __FUNCTION__, r.url.c_str(), r.elapsed));
        GetDetails();
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
            {"expires_in",      expires_in},
            {"userid",          patreon_userid},
        };
    }
    
    void AuthManager::from_json(const json& j) {
        if (j.contains("access_token")) {
            j.at("access_token").get_to(access_token);
        }

        if (j.contains("expires_in")) {
            j.at("expires_in").get_to(expires_in);
        }

        if (j.contains("userid")) {
            j.at("userid").get_to(patreon_userid);
        }
    }

    void AuthManager::Clear() {
        access_token.clear();
        expires_in = 0;
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