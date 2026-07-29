#pragma once

#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include <logger/logger.h>
#include <auth_manager/auth_manager.h>

namespace UIWindows {
    class UIAuthPopup
    {
    public:
        UIAuthPopup(UIAuthPopup& other) = delete;
        void operator=(const UIAuthPopup&) = delete;
        static UIAuthPopup* GetInstance();

        void Init();
        void Open();
        void Draw();

    private:
        static UIAuthPopup* pinstance_;
        static std::mutex mutex_;

    protected:
        bool show = false;
        bool auth_started = false;

        std::string window_name = "Authentication Required";

        UIAuthPopup();
        ~UIAuthPopup();
    };
}
