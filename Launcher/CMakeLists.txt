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

SET(OPENSSL_H
    "external/openssl/aes.h"
    "external/openssl/asn1.h"
    "external/openssl/asn1err.h"
    "external/openssl/asn1t.h"
    "external/openssl/asn1_mac.h"
    "external/openssl/async.h"
    "external/openssl/asyncerr.h"
    "external/openssl/bio.h"
    "external/openssl/bioerr.h"
    "external/openssl/blowfish.h"
    "external/openssl/bn.h"
    "external/openssl/bnerr.h"
    "external/openssl/buffer.h"
    "external/openssl/buffererr.h"
    "external/openssl/camellia.h"
    "external/openssl/cast.h"
    "external/openssl/cmac.h"
    "external/openssl/cmp.h"
    "external/openssl/cmperr.h"
    "external/openssl/cmp_util.h"
    "external/openssl/cms.h"
    "external/openssl/cmserr.h"
    "external/openssl/comp.h"
    "external/openssl/comperr.h"
    "external/openssl/conf.h"
    "external/openssl/conferr.h"
    "external/openssl/configuration.h"
    "external/openssl/conftypes.h"
    "external/openssl/conf_api.h"
    "external/openssl/core.h"
    "external/openssl/core_dispatch.h"
    "external/openssl/core_names.h"
    "external/openssl/core_object.h"
    "external/openssl/crmf.h"
    "external/openssl/crmferr.h"
    "external/openssl/crypto.h"
    "external/openssl/cryptoerr.h"
    "external/openssl/cryptoerr_legacy.h"
    "external/openssl/ct.h"
    "external/openssl/cterr.h"
    "external/openssl/decoder.h"
    "external/openssl/decodererr.h"
    "external/openssl/des.h"
    "external/openssl/dh.h"
    "external/openssl/dherr.h"
    "external/openssl/dsa.h"
    "external/openssl/dsaerr.h"
    "external/openssl/dtls1.h"
    "external/openssl/ebcdic.h"
    "external/openssl/ec.h"
    "external/openssl/ecdh.h"
    "external/openssl/ecdsa.h"
    "external/openssl/ecerr.h"
    "external/openssl/encoder.h"
    "external/openssl/encodererr.h"
    "external/openssl/engine.h"
    "external/openssl/engineerr.h"
    "external/openssl/err.h"
    "external/openssl/ess.h"
    "external/openssl/esserr.h"
    "external/openssl/evp.h"
    "external/openssl/evperr.h"
    "external/openssl/e_os2.h"
    "external/openssl/fipskey.h"
    "external/openssl/fips_names.h"
    "external/openssl/hmac.h"
    "external/openssl/http.h"
    "external/openssl/httperr.h"
    "external/openssl/idea.h"
    "external/openssl/kdf.h"
    "external/openssl/kdferr.h"
    "external/openssl/lhash.h"
    "external/openssl/macros.h"
    "external/openssl/md2.h"
    "external/openssl/md4.h"
    "external/openssl/md5.h"
    "external/openssl/mdc2.h"
    "external/openssl/modes.h"
    "external/openssl/objects.h"
    "external/openssl/objectserr.h"
    "external/openssl/obj_mac.h"
    "external/openssl/ocsp.h"
    "external/openssl/ocsperr.h"
    "external/openssl/opensslconf.h"
    "external/openssl/opensslv.h"
    "external/openssl/ossl_typ.h"
    "external/openssl/params.h"
    "external/openssl/param_build.h"
    "external/openssl/pem.h"
    "external/openssl/pem2.h"
    "external/openssl/pemerr.h"
    "external/openssl/pkcs12.h"
    "external/openssl/pkcs12err.h"
    "external/openssl/pkcs7.h"
    "external/openssl/pkcs7err.h"
    "external/openssl/proverr.h"
    "external/openssl/provider.h"
    "external/openssl/prov_ssl.h"
    "external/openssl/rand.h"
    "external/openssl/randerr.h"
    "external/openssl/rc2.h"
    "external/openssl/rc4.h"
    "external/openssl/rc5.h"
    "external/openssl/ripemd.h"
    "external/openssl/rsa.h"
    "external/openssl/rsaerr.h"
    "external/openssl/safestack.h"
    "external/openssl/seed.h"
    "external/openssl/self_test.h"
    "external/openssl/sha.h"
    "external/openssl/srp.h"
    "external/openssl/srtp.h"
    "external/openssl/ssl.h"
    "external/openssl/ssl2.h"
    "external/openssl/ssl3.h"
    "external/openssl/sslerr.h"
    "external/openssl/sslerr_legacy.h"
    "external/openssl/stack.h"
    "external/openssl/store.h"
    "external/openssl/storeerr.h"
    "external/openssl/symhacks.h"
    "external/openssl/tls1.h"
    "external/openssl/trace.h"
    "external/openssl/ts.h"
    "external/openssl/tserr.h"
    "external/openssl/txt_db.h"
    "external/openssl/types.h"
    "external/openssl/ui.h"
    "external/openssl/uierr.h"
    "external/openssl/whrlpool.h"
    "external/openssl/x509.h"
    "external/openssl/x509err.h"
    "external/openssl/x509v3.h"
    "external/openssl/x509v3err.h"
    "external/openssl/x509_vfy.h"
    "external/openssl/__DECC_INCLUDE_EPILOGUE.H"
    "external/openssl/__DECC_INCLUDE_PROLOGUE.H"
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
    "external/TextEditor.h"
    "external/AES/AES.h"
    "external/ImGuiFileDialog/ImGuiFileDialog.h"
)

SET(EXTERNAL_SRC
    "external/nlohmann/json.hpp"
    "external/nlohmann/json_fwd.hpp"
    "external/nlohmann/fifo_map.hpp"
    "external/tinyxml2.cpp"
    "external/TextEditor.cpp"
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
    "headers/auth_manager/auth_manager.h"
    "headers/files_manager/files_manager.h"

    "headers/fonts.h"
    "headers/core.h"
    "headers/localize.h"
    "headers/consts.h"
    "headers/options_id.h"
    "headers/server.h"

    "headers/context.h"
    "headers/hotkey_manager.h"
    "headers/injector.h"
    "headers/misc.h"

    "headers/ui/gui.h"
    "headers/ui/imgui_localize.h"
    "headers/ui/edit_hotkey_window.h"
    "headers/ui/create_hotkey_window.h"
    "headers/ui/settings_window.h"
    "headers/ui/locale_window.h"
    "headers/ui/status_window.h"
    "headers/ui/auth_window.h"
    "headers/ui/imgui_helper.h"
    
    ${EXTERNAL_H}
    ${OPENSSL_H}
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
    "source/auth_manager/auth_manager.cpp"
    "source/files_manager/files_manager.cpp"

    "source/core.cpp"
    "source/localize.cpp"
    "source/options_id.cpp"
    "source/server.cpp"
    "source/hotkey_manager.cpp"
    "source/injector.cpp"
    "source/misc.cpp"
    "source/ui/gui.cpp"
    "source/ui/imgui_localize.cpp"
    "source/ui/edit_hotkey_window.cpp"
    "source/ui/create_hotkey_window.cpp"
    "source/ui/settings_window.cpp"
    "source/ui/locale_window.cpp"
    "source/ui/status_window.cpp"
    "source/ui/auth_window.cpp"

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
target_link_libraries(${PROJECT_NAME} optimized ${EXTERNAL_LIB_RELEASE}/libcrypto.lib)
target_link_libraries(${PROJECT_NAME} optimized ${EXTERNAL_LIB_RELEASE}/libssl.lib)


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
