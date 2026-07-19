#include "SnapshotManager.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <string>

std::string SnapshotManager::resolveReason(
    const std::string& metric_name) const
{
    if (metric_name == "cpu.usage") {
        return "cpu_high";
    }

    if (metric_name == "mem.usage") {
        return "mem_high";
    }

    return "unknown_metric";
}

std::string SnapshotManager::makeSafeDir(
    const std::string& base) const
{
    std::string safe_dir = base;

    std::replace(
        safe_dir.begin(),
        safe_dir.end(),
        ':',
        '-'
    );

    std::replace(
        safe_dir.begin(),
        safe_dir.end(),
        ' ',
        '_'
    );

    return safe_dir;
}

std::string SnapshotManager::buildCommand(
    const std::string& snapshot_dir) const
{
    return "scripts/snapshot.sh \"" + snapshot_dir + "\"";
}

SnapshotResult SnapshotManager::take(
    const SnapshotRequest& request) const
{
    using Clock = std::chrono::steady_clock;

    SnapshotResult result;

    if (request.metric_name.empty()) {
        result.error_message = "metric name is empty";
        return result;
    }

    if (request.timestamp.empty()) {
        result.error_message = "timestamp is empty";
        return result;
    }

    result.reason = resolveReason(request.metric_name);

    result.snapshot_dir = makeSafeDir(
        "snapshots/" +
        request.timestamp +
        "_" +
        result.reason
    );

    result.command = buildCommand(result.snapshot_dir);

    const auto begin = Clock::now();

    try {
        result.return_code = std::system(result.command.c_str());
    } catch (const std::exception& e) {
        const auto end = Clock::now();

        result.cost_ms =
            std::chrono::duration<double, std::milli>(
                end - begin
            ).count();

        result.error_message =
            std::string("snapshot execution exception: ") + e.what();

        return result;
    } catch (...) {
        const auto end = Clock::now();

        result.cost_ms =
            std::chrono::duration<double, std::milli>(
                end - begin
            ).count();

        result.error_message =
            "snapshot execution failed with unknown exception";

        return result;
    }

    const auto end = Clock::now();

    result.cost_ms =
        std::chrono::duration<double, std::milli>(
            end - begin
        ).count();

    result.ok = (result.return_code == 0);

    if (!result.ok) {
        result.error_message =
            "snapshot script returned non-zero exit code";
    }

    return result;
}