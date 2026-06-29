#pragma once
#include <string>

struct SnapshotDecision {
    std::string cmd = "";
};

class SnapshotManager {
public:
    explicit SnapshotManager(std::string s , std::string ts) ;
    
    auto makeSafeDir(const std::string& base) -> std::string ;

    SnapshotDecision check() ;

private:
    std::string s_;
    std::string ts_;
    std::string snapshot_dir;

};
