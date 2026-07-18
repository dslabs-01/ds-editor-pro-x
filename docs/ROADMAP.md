# Roadmap

Realistic phase plan for a small team (3-5 engineers). Each phase should
end with something actually runnable and demoable, not just code written.

## Phase 0 — This scaffold (done)
Core data model (Timeline/Track/Clip/Project), Qt panel shell, CMake
build wiring, unit tests for edit-list logic. **Not yet playable.**

## Phase 1 — Real playback (6-10 weeks)
- Wire real FFmpeg decode in `DecodeEngine` (software path first)
- Basic `RenderGraph` compositing: single video track, no effects,
  straight cuts only
- `PlaybackController` driving `ViewerSurface` via OpenGL texture upload
- Import media, drop on timeline, play it back
- **Milestone: cut two clips together and watch them play.**

## Phase 2 — Real editing (6-8 weeks)
- Blade/ripple/roll/slip/slide tools fully wired to `TimelinePanel`
  mouse handling
- Multi-track compositing with opacity/blend modes
- Basic transitions (cross dissolve, wipe)
- Project save/load (JSON serialization of `Project`)
- Undo/redo via `Command` pattern end-to-end
- **Milestone: edit a real multi-clip sequence and save/reopen it.**

## Phase 3 — Color + Audio (8-10 weeks)
- OCIO integration in `ColorPipeline`, primary wheels + curves UI
- Waveform/vectorscope/histogram scopes
- JUCE-based audio mixer, EQ, basic effects
- LUT import/application
- **Milestone: grade a clip and mix audio with visible scopes.**

## Phase 4 — Export (4-6 weeks)
- `ExportQueue` background rendering with real FFmpeg encode
- Hardware encode (NVENC/QuickSync/AMF) with software fallback
- Delivery presets (YouTube, ProRes master, social vertical)
- Batch/queue UI
- **Milestone: export a graded, mixed sequence to MP4/MOV.**

## Phase 5 — First AI features (8-12 weeks)
Pick 3-4 highest-value features first, not all 27 at once:
- AI Scene Detection (shot boundary detection, feeds the blade tool)
- AI Auto Captions (Whisper via ONNX Runtime)
- AI Background Removal (segmentation model)
- AI Silence/Highlight detection for rough-cut assist
- **Milestone: auto-caption and auto-cut a raw interview.**

## Phase 6 — Motion graphics + titles (6-8 weeks)
- Title designer, text animation presets, lower thirds
- Shape layers, basic particle system

## Phase 7+ — Deferred to post-MVP
VFX compositing/tracking, node marketplace, cloud collaboration,
plugin SDK, multicam, HDR grading, ACES full workflow, remaining AI
features. These are real, valuable, and each is its own multi-month
effort — sequencing them after a working core product ships is what
makes the difference between "shipped v1" and "never shipped."

## What determines actual speed
Team size and whether you integrate existing libraries (FFmpeg, MLT,
OCIO, JUCE, ONNX Runtime) rather than writing decode/color/audio
engines from scratch. The stack in `docs/ARCHITECTURE.md` assumes
integration, which is why these phase estimates are weeks, not years —
but they're still real weeks of work, not something that completes in
one sitting.
