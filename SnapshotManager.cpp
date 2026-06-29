#include "SnapshotManager.h"
#include <algorithm>

SnapshotManager::SnapshotManager(std::string s , std::string ts) : s_(s), ts_(ts) {};

auto SnapshotManager::makeSafeDir(const std::string& base) -> std::string {
    std::string s = base;
    std::replace(s.begin(), s.end(), ':', '-');
    return s;
}

SnapshotDecision SnapshotManager::check() {
    std::string reason = s_.name == "cpu.usage" ? "cpu_high" : "mem_high";
    snapshot_dir = makeSafeDir("snapshots/" + ts_ + "_" + reason);
    std::string cmd = "scripts/snapshot.sh " + snapshot_dir;
    
}