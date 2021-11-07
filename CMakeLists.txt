cmake_minimum_required(VERSION 3.20)

project(ic)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(BE_PLATFORM_WINDOWS)
    add_compile_definitions(IMGUI_API=__declspec(dllexport))
elseif(BE_PLATFORM_LINUX)
    add_compile_definitions(IMGUI_API=)
endif(BE_PLATFORM_WINDOWS)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_library(GL gl)
find_package(OpenGL REQUIRED)

set(SOURCE_FILES
        src/main.cpp
        src/texture.h

        lib/stb_image/stb_image.h

        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui_extensions/ImguiDirectoryTreeView.h
        lib/imgui_extensions/ImguiDirectoryTreeView.cpp
        lib/imgui/imconfig.h
        lib/imgui/imgui.h
        lib/imgui/imgui_internal.h
        lib/imgui/imstb_rectpack.h
        lib/imgui/imstb_textedit.h
        lib/imgui/imstb_truetype.h
        lib/imgui/misc/cpp/imgui_stdlib.h
        lib/imgui/backends/imgui_impl_opengl3.cpp
        lib/imgui/backends/imgui_impl_opengl3.h
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_glfw.h
        )
include_directories(lib/imgui)
include_directories(lib/stb_image)

add_executable(ic ${SOURCE_FILES})

target_link_libraries(ic glfw OpenGL::GL m ${CMAKE_DL_LIBS})
target_include_directories(ic PRIVATE lib/imgui)