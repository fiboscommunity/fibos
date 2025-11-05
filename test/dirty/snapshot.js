const fibos = require('chain');
const fs = require('fs');
const path = require('path');

const snapshot_interval = 7200;
const max_snapshots = 3;

// Clean up old snapshot files, keep only the latest max_snapshots files
function cleanupOldSnapshots(snapshotPath) {
    try {
        if (!fs.exists(snapshotPath)) {
            return;
        }

        const files = fs.readdir(snapshotPath);
        const snapshotFiles = files.filter(file =>
            path.extname(file) === '.bin'
        );

        if (snapshotFiles.length <= max_snapshots) {
            return;
        }

        // Get file stats and sort by modification time (newest first)
        const fileStats = snapshotFiles.map(file => {
            const filePath = path.join(snapshotPath, file);
            const stat = fs.stat(filePath);
            return {
                name: file,
                path: filePath,
                mtime: stat.mtime
            };
        }).sort((a, b) => b.mtime - a.mtime);

        // Remove oldest files, keep only max_snapshots
        const filesToDelete = fileStats.slice(max_snapshots);
        filesToDelete.forEach(file => {
            try {
                fs.unlink(file.path);
                console.notice("deleted old snapshot:", file.name);
            } catch (e) {
                console.error("failed to delete snapshot file:", file.name, e.message);
            }
        });
    } catch (e) {
        console.error("failed to cleanup old snapshots:", e.message);
    }
}

module.exports = function (config) {
    const snapshotPath = path.resolve(fibos.data_dir, 'snapshots');

    fibos.load("producer_api");
    let last_snapshot_block_num = 0;

    setInterval(() => {
        const info = JSON.parse(fibos.post("/v1/chain/get_info", ""));
        const last_irreversible_block_num = info.last_irreversible_block_num;

        if (last_snapshot_block_num == 0)
            last_snapshot_block_num = last_irreversible_block_num;
        else {
            if (last_irreversible_block_num - last_snapshot_block_num >= snapshot_interval) {
                console.notice("create snapshot at block:", last_irreversible_block_num);
                const res = fibos.post("/v1/producer/create_snapshot", "");
                console.notice("create snapshot result:", res);
                last_snapshot_block_num = last_irreversible_block_num;

                // Clean up old snapshots after creating new one
                cleanupOldSnapshots(snapshotPath);
            }
        }
    }, 1000);

    // Check if state directory exists, if not, we need to restore from snapshot
    const statePath = path.resolve(fibos.data_dir, 'state');
    const stateFile = path.join(statePath, 'shared_memory.bin');
    const needsRestore = !fs.exists(statePath) || !fs.exists(stateFile) || fibos.is_dirty();
    
    if (needsRestore) {
        // Delete state directory if it exists
        console.notice("detected missing state directory or dirty state, attempting restore from snapshot");
        try {
            if (fs.exists(statePath)) {
                // First delete all files in the state directory
                const files = fs.readdir(statePath);
                files.forEach(file => {
                    const filePath = path.join(statePath, file);
                    const stat = fs.stat(filePath);
                    if (stat.isFile()) {
                        fs.unlink(filePath);
                        console.notice("deleted state file:", file);
                    } else if (stat.isDirectory()) {
                        // Recursively delete subdirectories
                        fs.rmdir(filePath);
                        console.notice("deleted state subdirectory:", file);
                    }
                });

                // Then delete the state directory itself
                fs.rmdir(statePath);
                console.notice("deleted state directory:", statePath);
            }
        } catch (e) {
            console.error("failed to delete state directory:", e.message);
        }

        try {
            if (fs.exists(snapshotPath)) {
                const files = fs.readdir(snapshotPath);
                const snapshotFiles = files.filter(file =>
                    path.extname(file) === '.bin'
                );

                if (snapshotFiles.length > 0) {
                    // Get file stats and sort by modification time (newest first)
                    const fileStats = snapshotFiles.map(file => {
                        const filePath = path.join(snapshotPath, file);
                        const stat = fs.stat(filePath);
                        return {
                            name: file,
                            path: filePath,
                            mtime: stat.mtime
                        };
                    }).sort((a, b) => b.mtime - a.mtime);

                    // Use the latest snapshot file
                    const latestSnapshot = fileStats[0];
                    config.snapshot = latestSnapshot.path;
                    console.notice("using latest snapshot:", latestSnapshot.name);
                    delete config['genesis-json'];
                } else {
                    console.error("no snapshot files found in:", snapshotPath);
                }
            } else {
                console.error("snapshots directory not found:", snapshotPath);
            }
        } catch (e) {
            console.error("failed to find latest snapshot:", e.message);
        }

        return true;
    }

    return false;
}
