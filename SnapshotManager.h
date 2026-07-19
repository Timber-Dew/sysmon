#pragma once

#include <string>

struct SnapshotRequest {
    std::string metric_name;
    std::string timestamp;
};

struct SnapshotResult {
    bool ok{false};

    int return_code{-1};
    double cost_ms{0.0};

    std::string reason;
    std::string snapshot_dir;
    std::string command;
    std::string error_message;
};

class SnapshotManager {
public:
    SnapshotManager() = default;

    SnapshotResult take(const SnapshotRequest& request) const;

private:
    std::string resolveReason(const std::string& metric_name) const;

    std::string makeSafeDir(const std::string& base) const;

    std::string buildCommand(const std::string& snapshot_dir) const;
};
