#pragma once
#include "core/Timeline.h"
#include "export/EncodeProfile.h"
#include <deque>
#include <functional>
#include <string>

namespace ds::exportmod {

enum class JobStatus { Queued, Rendering, Paused, Done, Failed, Cancelled };

struct ExportJob {
    std::string id;
    ds::core::Timeline* timeline; // non-owning
    EncodeProfile profile;
    std::string outputPath;
    JobStatus status = JobStatus::Queued;
    float progress = 0.0f; // 0..1
};

// Background export queue: renders jobs one (or a few, GPU-permitting) at a
// time without blocking the editing UI, per "Background rendering" +
// "Queue rendering" + "Batch export".
class ExportQueue {
public:
    using ProgressCallback = std::function<void(const std::string& jobId, float progress)>;
    using CompletionCallback = std::function<void(const std::string& jobId, bool success)>;

    std::string enqueue(ds::core::Timeline* timeline, EncodeProfile profile, std::string outputPath);
    void pause(const std::string& jobId);
    void resume(const std::string& jobId);
    void cancel(const std::string& jobId);

    void setProgressCallback(ProgressCallback cb) { onProgress_ = std::move(cb); }
    void setCompletionCallback(CompletionCallback cb) { onComplete_ = std::move(cb); }

    const std::deque<ExportJob>& jobs() const { return jobs_; }

private:
    void processNext(); // pulls next Queued job, drives RenderGraph + encoder

    std::deque<ExportJob> jobs_;
    int nextJobId_ = 0;
    ProgressCallback onProgress_;
    CompletionCallback onComplete_;
};

} // namespace ds::exportmod
