#include "export/ExportQueue.h"
#include <sstream>
#include <algorithm>

namespace ds::exportmod {

std::string ExportQueue::enqueue(ds::core::Timeline* timeline, EncodeProfile profile, std::string outputPath) {
    std::ostringstream idStream;
    idStream << "job_" << nextJobId_++;
    ExportJob job;
    job.id = idStream.str();
    job.timeline = timeline;
    job.profile = profile;
    job.outputPath = std::move(outputPath);
    jobs_.push_back(std::move(job));
    // TODO: kick processNext() on a background worker if not already running.
    return jobs_.back().id;
}

void ExportQueue::pause(const std::string& jobId) {
    auto it = std::find_if(jobs_.begin(), jobs_.end(), [&](auto& j) { return j.id == jobId; });
    if (it != jobs_.end() && it->status == JobStatus::Rendering) it->status = JobStatus::Paused;
}

void ExportQueue::resume(const std::string& jobId) {
    auto it = std::find_if(jobs_.begin(), jobs_.end(), [&](auto& j) { return j.id == jobId; });
    if (it != jobs_.end() && it->status == JobStatus::Paused) it->status = JobStatus::Queued;
}

void ExportQueue::cancel(const std::string& jobId) {
    auto it = std::find_if(jobs_.begin(), jobs_.end(), [&](auto& j) { return j.id == jobId; });
    if (it != jobs_.end()) it->status = JobStatus::Cancelled;
}

void ExportQueue::processNext() {
    // TODO: pull next Queued job, drive RenderGraph::renderFrame across the
    // full duration, feed frames into an FFmpeg encoder (matching
    // profile.videoCodec/container, using GPU encode when
    // profile.useGpuEncode and a compatible encoder exists), reporting
    // progress via onProgress_ and firing onComplete_ when done or on error.
}

} // namespace ds::exportmod
