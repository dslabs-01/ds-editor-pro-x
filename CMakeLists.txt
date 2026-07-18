cmake_minimum_required(VERSION 3.20)
project(DSEditorProX VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---------------------------------------------------------------------------
# Build type
# ---------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ---------------------------------------------------------------------------
# Dependencies
#   - Qt6 (Widgets, OpenGL, Multimedia) for the UI shell
#   - FFmpeg (libavcodec/libavformat/libavutil/libswscale) for decode/encode
#   - MLT framework for timeline/track/transition engine
#   - OpenColorIO for color management
# These must be installed on the build machine (see docs/BUILD.md).
# ---------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets Multimedia)
find_package(PkgConfig REQUIRED)

pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec libavformat libavutil libswscale libswresample
)

pkg_check_modules(MLT QUIET IMPORTED_TARGET mlt-framework)
pkg_check_modules(OCIO QUIET IMPORTED_TARGET OpenColorIO)

# ---------------------------------------------------------------------------
# Core engine (no Qt dependency — pure C++ so it is portable / testable)
# ---------------------------------------------------------------------------
add_library(ds_core STATIC
    src/core/Timeline.cpp
    src/core/Track.cpp
    src/core/Clip.cpp
    src/core/Project.cpp
    src/core/MediaAsset.cpp
    src/core/UndoStack.cpp
)
target_include_directories(ds_core PUBLIC src)

add_library(ds_engine STATIC
    src/engine/DecodeEngine.cpp
    src/engine/RenderGraph.cpp
    src/engine/ProxyManager.cpp
    src/engine/PlaybackController.cpp
)
target_include_directories(ds_engine PUBLIC src)
target_link_libraries(ds_engine PUBLIC ds_core PkgConfig::FFMPEG)
if(MLT_FOUND)
    target_link_libraries(ds_engine PUBLIC PkgConfig::MLT)
    target_compile_definitions(ds_engine PUBLIC DS_HAVE_MLT=1)
endif()

add_library(ds_color STATIC
    src/color/ColorPipeline.cpp
    src/color/Scopes.cpp
)
target_include_directories(ds_color PUBLIC src)
target_link_libraries(ds_color PUBLIC ds_core)
if(OCIO_FOUND)
    target_link_libraries(ds_color PUBLIC PkgConfig::OCIO)
    target_compile_definitions(ds_color PUBLIC DS_HAVE_OCIO=1)
endif()

add_library(ds_export STATIC
    src/export/ExportQueue.cpp
    src/export/EncodeProfile.cpp
)
target_include_directories(ds_export PUBLIC src)
target_link_libraries(ds_export PUBLIC ds_core ds_engine)

# ---------------------------------------------------------------------------
# Application (Qt UI shell)
# ---------------------------------------------------------------------------
add_executable(DSEditorProX
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/TimelinePanel.cpp
    src/ui/ViewerPanel.cpp
    src/ui/MediaBinPanel.cpp
    src/ui/InspectorPanel.cpp
)
target_link_libraries(DSEditorProX PRIVATE
    Qt6::Widgets Qt6::OpenGLWidgets Qt6::Multimedia
    ds_core ds_engine ds_color ds_export
)

# Standalone CLI proof that DecodeEngine performs real decode against real
# files — useful for smoke-testing a build without needing the full Qt UI.
add_executable(decode_probe tests/decode_probe.cpp)
target_link_libraries(decode_probe PRIVATE ds_engine)

add_executable(seek_probe tests/seek_probe.cpp)
target_link_libraries(seek_probe PRIVATE ds_engine)

add_executable(composite_probe tests/composite_probe.cpp)
target_link_libraries(composite_probe PRIVATE ds_core ds_engine)

enable_testing()
add_subdirectory(tests)
