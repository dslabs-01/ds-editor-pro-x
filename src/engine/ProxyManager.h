#pragma once
#include "core/MediaAsset.h"
#include <functional>
#include <atomic>

namespace ds::engine {

enum class ProxyQuality { Quarter, Half, Full };

// Runs in a background thread pool on import. Decides whether a proxy is
// needed at all (skips it on workstation-class hardware with the source
// codec already fast to decode), matching "Smart proxy generation".
class ProxyManager {
public:
    struct Config {
        ProxyQuality quality = ProxyQuality::Half;
        std::string codec = "h264_nvenc"; // falls back to libx264 if no GPU encoder
        int maxConcurrentJobs = 2;        // tuned down further on 4GB RAM machines
    };

    explicit ProxyManager(Config config);

    // Async; progress/completion delivered via callback so the UI (media
    // bin badges) can reflect status without polling.
    using ProgressCallback = std::function<void(const std::string& mediaAssetId, float pct)>;
    using CompletionCallback = std::function<void(const std::string& mediaAssetId, bool success)>;

    void requestProxy(ds::core::MediaAsset& asset,
                       ProgressCallback onProgress,
                       CompletionCallback onComplete);

    void cancel(const std::string& mediaAssetId);

private:
    Config config_;
    std::atomic<int> activeJobs_{0};
};

} // namespace ds::engine
