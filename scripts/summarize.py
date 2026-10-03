#!/usr/bin/env python3
"""Convierte los registros TIME/THREAD de results/raw en CSV con medianas, speedup y eficiencia."""
import csv
import pathlib
import statistics
from collections import defaultdict

ROOT = pathlib.Path(__file__).resolve().parent.parent
RAW = ROOT / "results" / "raw"
RES = ROOT / "results"

KEYS = ["design", "image", "filter", "threads", "nodes", "rank"]
NUMS = ["read_ms", "comm_ms", "filter_wall_ms", "filter_cpu_ms", "write_ms",
        "total_wall_ms", "total_cpu_ms"]


def parse(path, prefix):
    rows = []
    for line in path.read_text().splitlines():
        if line.startswith(prefix + " "):
            rows.append(dict(tok.split("=", 1) for tok in line.split()[1:]))
    return rows


def medians(rows, keys, nums):
    groups = defaultdict(list)
    for r in rows:
        groups[tuple(r[k] for k in keys)].append(r)
    out = []
    for key, grp in sorted(groups.items()):
        row = dict(zip(keys, key))
        row["reps"] = len(grp)
        for n in nums:
            row[n] = round(statistics.median(float(g[n]) for g in grp), 3)
        out.append(row)
    return out


def write_csv(path, rows, fields):
    with open(path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def main():
    tables = {}
    for name in ["d2_seq", "d3_threads", "d3_omp", "d4_mpi"]:
        path = RAW / f"{name}.txt"
        if not path.exists():
            continue
        rows = medians(parse(path, "TIME"), KEYS, NUMS)
        write_csv(RES / f"{name}.csv", rows, KEYS + ["reps"] + NUMS)
        tables[name] = rows

    threads_raw = RAW / "d3_threads.txt"
    if threads_raw.exists():
        rows = medians(parse(threads_raw, "THREAD"), ["img", "filter", "region"], ["cpu_ms"])
        write_csv(RES / "d3_threads_cpu.csv", rows, ["img", "filter", "region", "reps", "cpu_ms"])

    seq = {(r["image"], r["filter"]): r for r in tables.get("d2_seq", [])}
    summary = []

    def add(design, row, filter_wall, total_wall, workers):
        base = seq.get((row["image"], row["filter"]))
        if base is None:
            return
        sp_filter = base["filter_wall_ms"] / filter_wall if filter_wall > 0 else 0.0
        sp_total = base["total_wall_ms"] / total_wall if total_wall > 0 else 0.0
        summary.append({
            "design": design, "image": row["image"], "filter": row["filter"],
            "workers": workers, "filter_wall_ms": filter_wall, "total_wall_ms": total_wall,
            "speedup_filter": round(sp_filter, 3), "speedup_total": round(sp_total, 3),
            "efficiency_filter": round(sp_filter / workers, 3),
        })

    for r in tables.get("d2_seq", []):
        add("sequential", r, r["filter_wall_ms"], r["total_wall_ms"], 1)
    for r in tables.get("d3_threads", []):
        add("threads", r, r["filter_wall_ms"], r["total_wall_ms"], int(r["threads"]))
    for r in tables.get("d3_omp", []):
        add("openmp", r, r["filter_wall_ms"], r["total_wall_ms"], int(r["threads"]))

    mpi = defaultdict(list)
    for r in tables.get("d4_mpi", []):
        mpi[(r["image"], r["filter"], r["nodes"])].append(r)
    for (_, _, nodes), grp in sorted(mpi.items()):
        root = next(g for g in grp if g["rank"] == "0")
        slowest = max(g["filter_wall_ms"] for g in grp)
        add("mpi", root, slowest, root["total_wall_ms"], int(nodes))

    write_csv(RES / "summary.csv", summary,
              ["design", "image", "filter", "workers", "filter_wall_ms", "total_wall_ms",
               "speedup_filter", "speedup_total", "efficiency_filter"])
    print(f"summary.csv: {len(summary)} filas")


if __name__ == "__main__":
    main()
