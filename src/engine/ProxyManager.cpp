#include "engine/ProxyManager.h"

namespace ds::engine {

ProxyManager::ProxyManager(Config config) : config_(std::move(config)) {}

void ProxyManager::requestProxy(ds::core::MediaAsset& asset,
                                  ProgressCallback onProgress,
                                  CompletionCallback onComplete) {
    // TODO: spawn a worker (thread pool, capped by config_.maxConcurrentJobs)
    // that runs an FFmpeg transcode: source -> proxy codec at
    // ProxyQuality-derived resolution, writing next to the source in a
    // .ds-proxies/ cache directory. Reports progress via onProgress and
    // sets asset.setProxyPath(...) + calls onComplete on the main thread.
    //
    // Hardware detection: prefer NVENC/QuickSync/AMF; if none available or
    // the encoder init fails, retry once with libx264 (CPU fallback), per
    // the "GPU acceleration / CPU fallback" requirement.
    (void)asset; (void)onProgress; (void)onComplete;
}

void ProxyManager::cancel(const std::string& mediaAssetId) {
    // TODO: signal the worker for mediaAssetId to stop and clean up partial output.
    (void)mediaAssetId;
}

} // namespace ds::engine
