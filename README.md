# Sysmon

Sysmon is a lightweight Linux system monitoring service written in C++.

This project is built as an **engineering-oriented practice repo** to evolve system-level capabilities step by step:
stable scheduling, observability (metrics/logs/snapshots), alert engineering, reliability isolation, and metrics expansion.

---

## Features

- Periodic CPU / memory metrics collection
- Stable periodic tick based on `steady_clock` + `sleep_until`
- Service loop cost metric: `service.loop_cost_ms`
- Structured metrics output in **JSONL** (`metrics.jsonl`)
- Alert event logging (`event=alarm`) with automatic system snapshot under `snapshots/`
- Minimal dependencies, suitable for embedded Linux / WSL / dev boards

---

## Output Examples

### Metrics (JSONL)

Each tick writes one line into `metrics.jsonl`:

```json
{"ts":"2025-12-19T15:42:32.723+09:00","level":"INFO","event":"metrics","metrics":{"cpu.usage":0.0277,"mem.usage":6.87,"service.loop_cost_ms":0.21},"unit":{"cpu.usage":"%","mem.usage":"%","service.loop_cost_ms":"ms"}}
