#include "ui/auth_window.h"

namespace UIWindows {
    UIAuthPopup::UIAuthPopup() {}

    UIAuthPopup::~UIAuthPopup() {}

    void UIAuthPopup::Init() {

    }

    void UIAuthPopup::Open() {
        show = true;
        auth_started = false;
    }

    void UIAuthPopup::Draw() {
        if (!show)  return;

        if (!ImGui::IsPopupOpen(window_name.c_str()))
            ImGui::OpenPopup(window_name.c_str());

        auto viewport = ImGui::GetMainViewport();

        // Center
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImVec2 sz = viewport->Size;
        sz.x *= 0.40f;
        sz.y = 125.0f;
        ImGui::SetNextWindowSize(sz, ImGuiCond_Always);

        if (ImGui::BeginPopupModal(window_name.c_str(), &show)) {
            auto avail_space = ImGui::GetContentRegionAvail();

            if (auth_started) {
                ImGui::Text("Login with Patreon in progress...");
            }
            else {
                ImGui::Text("Login with Patreon");
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Login with Patreon to continue using the Live Editor");
                }

                ImGui::SetCursorPos(ImVec2(
                    0.0f,
                    avail_space.y - 10.0f
                ));

                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(138, 119, 23, 255));
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(192, 246, 241, 255));
                if (ImGui::Button("Login with Patreon", ImVec2(-FLT_MIN, 0.0f))) {
                    LE::AuthManager::GetInstance()->DoLogin();
                    auth_started = true;
                }
                ImGui::PopStyleColor(2);
            }

            ImGui::EndPopup();
        }

        auto login_status = LE::AuthManager::GetInstance()->GetLoginStatus();
        if (
            login_status == LE::LOGIN_STATUS::TOKEN_EXPIRED ||
            login_status == LE::LOGIN_STATUS::NO_ACCESS ||
            login_status == LE::LOGIN_STATUS::HAS_ACCESS
        ) {
            show = false;
        }
    }

    UIAuthPopup* UIAuthPopup::GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            pinstance_ = new UIAuthPopup();

        return pinstance_;
    }
}

UIWindows::UIAuthPopup* UIWindows::UIAuthPopup::pinstance_{ nullptr };
std::mutex UIWindows::UIAuthPopup::mutex_;