# Build Instructions (Windows primary target, Linux for dev)

## Dependencies

- CMake >= 3.20
- A C++20 compiler (MSVC 2022 on Windows, GCC 12+/Clang 15+ on Linux)
- Qt6 (Widgets, OpenGLWidgets, Multimedia) — install via the Qt online
  installer or `vcpkg install qt6`
- FFmpeg dev libraries (libavcodec, libavformat, libavutil, libswscale,
  libswresample) — `vcpkg install ffmpeg` or system package manager
- Optional: MLT framework (`vcpkg install mlt`) — build works without
  it, falls back to a simpler internal compositor path
- Optional: OpenColorIO (`vcpkg install opencolorio`) — build works
  without it, color pipeline stubs stay inert
- Optional: GTest (`vcpkg install gtest`) for `ds_core_tests`

## Windows (recommended: vcpkg + Visual Studio)

```powershell
git clone <this-repo>
cd ds-editor-pro-x
vcpkg install qt6-base ffmpeg mlt opencolorio gtest
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## Linux (development)

```bash
sudo apt install qt6-base-dev libqt6opengl6-dev ffmpeg libavcodec-dev \
    libavformat-dev libavutil-dev libswscale-dev libswresample-dev \
    libgtest-dev cmake build-essential pkg-config
cmake -B build -S .
cmake --build build -j$(nproc)
ctest --test-dir build
```

## Notes

- The app will build and launch with just Qt6 + FFmpeg — MLT and OCIO
  are additive (`DS_HAVE_MLT` / `DS_HAVE_OCIO` compile defines gate the
  code paths that use them).
- `tests/CMakeLists.txt` silently skips the test target if GTest isn't
  found, so it never blocks a normal app build.
