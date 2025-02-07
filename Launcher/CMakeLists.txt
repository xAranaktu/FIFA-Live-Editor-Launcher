# CMakeList.txt : CMake project for Launcher, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

SET(ImGUI_H
    "external/imgui/imconfig.h"
    "external/imgui/imgui.h"
    "external/imgui/imgui_internal.h"
    "external/imgui/imstb_rectpack.h"
    "external/imgui/imstb_textedit.h"
    "external/imgui/imstb_truetype.h"
    "external/imgui/imgui_impl_dx11.h"
    "external/imgui/imgui_impl_win32.h"
    "external/imgui/imgui_stdlib.h"
)

# Local source files here
SET(ImGUI_SRC
    "external/imgui/imgui.cpp"
    "external/imgui/imgui_demo.cpp"
    "external/imgui/imgui_draw.cpp"
    "external/imgui/imgui_widgets.cpp"
    "external/imgui/imgui_tables.cpp"
    "external/imgui/imgui_impl_dx11.cpp"
    "external/imgui/imgui_impl_win32.cpp"
    "external/imgui/imgui_stdlib.cpp"
)

SET(CPR_H
    "external/cpr/accept_encoding.h"
    "external/cpr/api.h"
    "external/cpr/async.h"
    "external/cpr/auth.h"
    "external/cpr/bearer.h"
    "external/cpr/body.h"
    "external/cpr/buffer.h"
    "external/cpr/callback.h"
    "external/cpr/cert_info.h"
    "external/cpr/connect_timeout.h"
    "external/cpr/cookies.h"
    "external/cpr/cpr.h"
    "external/cpr/cprtypes.h"
    "external/cpr/cprver.h"
    "external/cpr/curlholder.h"
    "external/cpr/curl_container.h"
    "external/cpr/error.h"
    "external/cpr/file.h"
    "external/cpr/http_version.h"
    "external/cpr/interceptor.h"
    "external/cpr/interface.h"
    "external/cpr/limit_rate.h"
    "external/cpr/local_port.h"
    "external/cpr/local_port_range.h"
    "external/cpr/low_speed.h"
    "external/cpr/multipart.h"
    "external/cpr/parameters.h"
    "external/cpr/payload.h"
    "external/cpr/proxies.h"
    "external/cpr/proxyauth.h"
    "external/cpr/range.h"
    "external/cpr/redirect.h"
    "external/cpr/reserve_size.h"
    "external/cpr/response.h"
    "external/cpr/session.h"
    "external/cpr/singleton.h"
    "external/cpr/ssl_ctx.h"
    "external/cpr/ssl_options.h"
    "external/cpr/status_codes.h"
    "external/cpr/threadpool.h"
    "external/cpr/timeout.h"
    "external/cpr/unix_socket.h"
    "external/cpr/user_agent.h"
    "external/cpr/util.h"
    "external/cpr/verbose.h"
)

SET (ZLIB_H
    "external/zlib/zlib.h"
    "external/zlib/zconf.h"
)

SET (CURL_H
    "external/curl/curl.h"
    "external/curl/curlver.h"
    "external/curl/easy.h"
    "external/curl/header.h"
    "external/curl/mprintf.h"
    "external/curl/multi.h"
    "external/curl/options.h"
    "external/curl/stdcheaders.h"
    "external/curl/system.h"
    "external/curl/typecheck-gcc.h"
    "external/curl/urlapi.h"
    "external/curl/websockets.h"
)

SET(EXTERNAL_H
    "external/IconsFontAwesome6.h"
    "external/tinyxml2.h"
    "external/AES/AES.h"
    "external/ImGuiFileDialog/ImGuiFileDialog.h"
)

SET(EXTERNAL_SRC
    "external/nlohmann/json.hpp"
    "external/nlohmann/json_fwd.hpp"
    "external/nlohmann/fifo_map.hpp"
    "external/tinyxml2.cpp"
    "external/AES/AES.cpp"
    "external/ImGuiFileDialog/ImGuiFileDialog.cpp"
)

SET(ALL_HEADERS
    "headers/legacy_file_structure.h"

    "headers/logger/logger.h"
    "headers/logger/enum.h"
    "headers/logger/const.h"

    "headers/config/config.h"
    "headers/config/settings.h"
    "headers/config/values.h"
    "headers/config/ui_styles.h"

    "headers/version_manager/version_manager.h"
    "headers/files_manager/files_manager.h"

    "headers/fonts.h"
    "headers/core.h"
    "headers/localize.h"
    "headers/consts.h"
    "headers/options_id.h"

    "headers/context.h"
    "headers/hotkey_manager.h"
    "headers/injector.h"
    "headers/misc.h"

    "headers/ui/gui.h"
    "headers/ui/imgui_localize.h"
    "headers/ui/edit_hotkey_window.h"
    "headers/ui/settings_window.h"
    "headers/ui/locale_window.h"
    "headers/ui/status_window.h"
    "headers/ui/imgui_helper.h"
    
    ${EXTERNAL_H}
    ${ZLIB_H}
    ${CURL_H}
    ${CPR_H}
    ${ImGUI_H}
)

SET(ALL_CPP
    "source/logger/logger.cpp"
    "source/config/config.cpp"
    "source/config/settings.cpp"
    "source/config/values.cpp"

    "source/version_manager/version_manager.cpp"
    "source/files_manager/files_manager.cpp"

    "source/core.cpp"
    "source/localize.cpp"
    "source/options_id.cpp"
    "source/hotkey_manager.cpp"
    "source/injector.cpp"
    "source/misc.cpp"
    "source/ui/gui.cpp"
    "source/ui/imgui_localize.cpp"
    "source/ui/edit_hotkey_window.cpp"
    "source/ui/settings_window.cpp"
    "source/ui/locale_window.cpp"
    "source/ui/status_window.cpp"

    "main.cpp"

    ${EXTERNAL_SRC}
    ${ImGUI_SRC}
)

SET(APP_ICON "appicon.rc")

add_executable(${PROJECT_NAME} WIN32 ${ALL_CPP} ${ALL_HEADERS} ${APP_ICON})

target_link_libraries(${PROJECT_NAME} ${WinSDK})

target_include_directories(${PROJECT_NAME} PRIVATE "external/")
target_include_directories(${PROJECT_NAME} PRIVATE "external/AES/")
target_include_directories(${PROJECT_NAME} PRIVATE "external/imgui/")
target_include_directories(${PROJECT_NAME} PRIVATE "headers/")
target_include_directories(${PROJECT_NAME} PRIVATE "headers/ui/")
target_include_directories(${PROJECT_NAME} PRIVATE ${WinSDK})

# add WALL + PDB flags
if(MSVC)
  # force static lib
  set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()
get_filename_component(EXTERNAL_LIB_RELEASE "external/lib/Release" ABSOLUTE)
target_link_libraries(${PROJECT_NAME} optimized ${EXTERNAL_LIB_RELEASE}/zlib.lib)
target_link_libraries(${PROJECT_NAME} optimized ${EXTERNAL_LIB_RELEASE}/cpr.lib)
target_link_libraries(${PROJECT_NAME} optimized ${EXTERNAL_LIB_RELEASE}/libcurl.lib)


set(JSON_BuildTests OFF CACHE INTERNAL "")

# Generate PDB for Release (x64-ReleasePDB)
if (GEN_PDB)
  message("GEN_PDB")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

if (IS_DEV)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DIS_DEV")
endif()

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS    "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS")
