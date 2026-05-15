#!/usr/bin/env bash
set -euo pipefail

OUT_DIR="${1:-snapshots/$(date +%Y%m%d_%H%M%S)}"
mkdir -p "$OUT_DIR"

echo "[snapshot] writing to $OUT_DIR"

date > "$OUT_DIR/time.txt"
uname -a > "$OUT_DIR/uname.txt" || true
uptime > "$OUT_DIR/uptime.txt" || true

# CPU / load
cat /proc/loadavg > "$OUT_DIR/loadavg.txt" || true
vmstat 1 5 > "$OUT_DIR/vmstat.txt" 2>&1 || true
top -b -n 1 > "$OUT_DIR/top.txt" 2>&1 || true
ps aux --sort=-%cpu | head -n 50 > "$OUT_DIR/ps_cpu_top50.txt" 2>&1 || true

# MEM
free -m > "$OUT_DIR/free_m.txt" 2>&1 || true
cat /proc/meminfo > "$OUT_DIR/meminfo.txt" 2>&1 || true
ps aux --sort=-%mem | head -n 50 > "$OUT_DIR/ps_mem_top50.txt" 2>&1 || true

# IO (optional)
if command -v iostat >/dev/null 2>&1; then
  iostat -xz 1 3 > "$OUT_DIR/iostat_xz.txt" 2>&1 || true
else
  echo "iostat not found" > "$OUT_DIR/iostat_xz.txt"
fi

# Kernel log tail
dmesg -T | tail -n 200 > "$OUT_DIR/dmesg_tail200.txt" 2>&1 || true

echo "[snapshot] done"

