# DS Editor Pro X — Architecture

## Layered design

```
src/ui/       Qt6 widgets — dockable panels, painting, input handling.
              Zero business logic; talks to core/ and engine/ only.
src/core/     Pure C++ data model (Project, Timeline, Track, Clip,
              MediaAsset, UndoStack). No Qt, no FFmpeg — fully unit
              testable, and reusable if a headless/CLI/render-farm
              mode is ever needed.
src/engine/   Decode (FFmpeg), compositing (RenderGraph, GPU),
              proxy generation, playback clock. Depends on core/.
src/color/    OCIO-backed grading pipeline + scopes. Depends on core/.
src/export/   Encode profiles + background export queue. Depends on
              core/ and engine/.
```

Dependency direction is strictly one-way: `ui -> {engine, color, export} -> core`.
`core` never depends on anything above it. This is what makes the data
model testable without a display, and is a precondition for any future
headless/cloud-rendering mode.

## Why these libraries

- **FFmpeg**: decode/encode for every format in the export list, plus
  hardware acceleration (NVDEC/NVENC, QuickSync, AMF) with automatic
  software fallback.
- **MLT framework** (optional, `DS_HAVE_MLT`): battle-tested
  transition/filter primitives so RenderGraph doesn't reinvent
  compositing math from zero. Used opportunistically — the app still
  builds and runs without it.
- **OpenColorIO** (optional, `DS_HAVE_OCIO`): color-managed grading,
  ACES workflow, LUT application — the same library used in film
  pipelines, not a hand-rolled color math approximation.
- **Qt6 (Widgets + OpenGLWidgets)**: native performance for real-time
  timeline painting and GPU preview, cross-platform, mature docking
  system for the "fully customizable workspace" requirement.
- **JUCE** (audio, integrated in Phase 3): industry-standard for the
  mixer/EQ/compression/limiter chain.
- **ONNX Runtime** (AI, integrated in Phase 5): run pretrained models
  (Whisper for captions/translation, segmentation models for
  background/object removal, RIFE for frame interpolation) without
  owning model training.

## Performance model

- **core::FrameTime** is an integer frame count, never a float
  timestamp — eliminates drift on long timelines and keeps trim/roll
  math exact.
- **RenderGraph::renderFrame** takes a `playbackResolutionScale`
  parameter. `PlaybackController::chooseResolutionScale()` drops this
  under CPU/GPU load so playback stays smooth on 4GB machines instead
  of dropping frames — this is the "Dynamic playback resolution"
  requirement's actual mechanism.
- **ProxyManager** runs on import, off the UI thread, and is skippable
  entirely on workstation-class hardware — "smart" means deciding
  *not* to generate a proxy when the source is already fast to decode.
- **TimelinePanel** is a hand-painted QWidget, not an item-view model,
  specifically so redraw cost during scrubbing is bounded by visible
  clips only (off-screen culling in `paintEvent`).

## Undo/redo and future collaboration

Every mutation should be expressed as a `core::Command` pushed through
`UndoStack`. This is deliberate: it's the same mechanism that will
later support autosave diffing, version history, and real-time
collaboration (each Command becomes a replayable, mergeable operation)
without a rewrite — plan for it now even though Phase 1 only wires
undo/redo locally.

## What's stubbed vs. real in this scaffold

Real: the full core data model (Timeline/Track/Clip/Project/MediaAsset)
with working trim/slip/roll/magnetic-timeline logic and unit tests;
the Qt panel shell and dark theme; CMake build wiring for all
dependencies.

Stubbed (marked `TODO` at the call site): FFmpeg encode calls, GPU
shader compositing, OCIO processor invocation, AI model inference.
These are integration work against the libraries above, not
undesigned — the surrounding architecture is built to receive them.

**`engine::DecodeEngine` is real, not stubbed.** It performs actual
`avformat_open_input` / `avcodec_send_packet` / `avcodec_receive_frame`
/ `sws_scale` / `av_seek_frame` calls against real files. Verified two ways:

- `decode_probe`: confirmed correct resolution, frame rate, byte count,
  and PTS for a decoded frame, and that pixel data is genuine image
  content (not zeroed memory).
- `seek_probe`: decodes a deliberately out-of-order sequence of frames
  (forward, big forward jump, backward jump, near-end) from a single
  open() call. Jumps of 800-1400 frames complete in 2-7ms, confirming
  real keyframe-based seeking rather than an O(n) scan from frame 0.

This process caught a real bug: the last 2-3 frames of every clip were
undecodable because H.264's B-frame reordering leaves frames buffered
inside the decoder that are never emitted unless you explicitly signal
end-of-stream by sending a null packet. Fixed in
`DecodeEngine::decodeNextFrameInternal` by draining the decoder on EOF;
verified by re-running `decode_probe` against the exact frame indices
that failed before the fix.

Current known limitation: seek granularity depends on keyframe
interval (GOP size) — a request lands on the nearest frame at or after
the target if exact-frame decode isn't reached within the scan bound,
rather than guaranteeing exact-frame accuracy on every codec. This is
fine for scrubbing/preview; frame-exact trim-point decode for export
needs a follow-up pass once export is implemented.

**`engine::RenderGraph` is real, not stubbed.** It walks the Timeline's
video tracks bottom-to-top, finds the active clip per track via
`Track::clipAt`, decodes that clip's actual source frame (computed
from the clip's timeline position and trim in-point) via a pooled
`DecodeEngine`, resizes to the composite resolution with `sws_scale`,
and alpha-over composites tracks together. It takes a
`MediaPathResolver` callback rather than depending on `core::Project`
directly, keeping the engine/core dependency direction one-way.

Verified with `composite_probe`: a real two-track Timeline (colorbars
on the bottom track for the full duration, a distinct SMPTE bars clip
on the top track covering only frames 30-50) was rendered at three
frames. The output correctly showed the bottom pattern before/after
the overlay window and the top pattern during it — confirmed visually,
not just by non-zero pixel counts, ruling out a coincidental match.

Not yet wired: `clip->effects()` (color/VFX chain) is skipped before
compositing — `ColorPipeline::applyToFrame` is still a no-op stub, so
this is the next real integration point once color grading moves off
the stub list. GPU-based compositing (vs. the current CPU alpha blend)
is deferred until the app has an active OpenGL/Vulkan context to
target; the CPU path was sufficient to prove the data path end-to-end.
