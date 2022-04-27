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

SET(ALL_HEADERS
    "headers/core.h"
    "headers/context.h"
    "headers/logger.h"
    "headers/injector.h"
    "headers/ui/gui.h"
    
    ${ImGUI_H}
)

SET(ALL_CPP
    "source/core.cpp"
    "source/logger.cpp"
    "source/injector.cpp"
    "source/ui/gui.cpp"

    "main.cpp"

    ${ImGUI_SRC}
)


add_executable(${PROJECT_NAME} WIN32 ${ALL_CPP} ${ALL_HEADERS})

target_link_libraries(${PROJECT_NAME} ${WinSDK})

target_include_directories(${PROJECT_NAME} PRIVATE "external/imgui/")
target_include_directories(${PROJECT_NAME} PRIVATE "headers/")
target_include_directories(${PROJECT_NAME} PRIVATE "headers/ui/")
target_include_directories(${PROJECT_NAME} PRIVATE ${WinSDK})


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