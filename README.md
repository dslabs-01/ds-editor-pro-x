# DS Editor Pro X

A professional desktop video editor scaffold: NLE core data model, Qt6
UI shell, and integration points for FFmpeg (decode/encode), MLT
(compositing), OpenColorIO (grading), JUCE (audio), and ONNX Runtime
(AI features).

**Status: architectural scaffold, not a working editor yet.** The core
edit-list logic (timeline/track/clip, trim/ripple/roll/slip, magnetic
timeline) is real and unit-tested. Playback, compositing, color, audio,
export, and AI are wired up as typed interfaces with `TODO`-marked
integration points against real libraries — see `docs/ROADMAP.md` for
what's next and in what order.

## Layout

```
src/core/     Data model — Project, Timeline, Track, Clip, MediaAsset, UndoStack
src/engine/   Decode, GPU compositing, proxy generation, playback clock
src/color/    OCIO grading pipeline, scopes (waveform/vectorscope/histogram)
src/export/   Encode profiles, background export queue
src/ui/       Qt6 dockable panel shell — MainWindow, Viewer, Timeline, MediaBin, Inspector
tests/        GTest unit tests for the core edit-list logic
docs/         Architecture, roadmap, build instructions
```

## Quick start

See `docs/BUILD.md` for full dependency setup (Windows/vcpkg and Linux
paths). Short version:

```bash
cmake -B build -S .
cmake --build build -j
ctest --test-dir build   # runs the core logic unit tests
```

## Read next

- `docs/ARCHITECTURE.md` — why this stack, layering rules, what's real vs. stubbed
- `docs/ROADMAP.md` — phase-by-phase plan from "builds and launches" to a usable v1
