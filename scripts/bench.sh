#!/usr/bin/env bash
# Ejecuta todas las mediciones y genera los CSV de results/.
# Correr dentro de node1 como usuario mpi. REPS=5 por defecto.
set -euo pipefail
cd "$(dirname "$0")/.."

REPS="${REPS:-5}"
OUT=output/bench
RAW=results/raw
mkdir -p "$OUT" "$RAW"
make -s all

: > "$RAW/d2_seq.txt"
: > "$RAW/d3_threads.txt"
: > "$RAW/d3_omp.txt"
: > "$RAW/d4_mpi.txt"

run() {
  local log="$1" img="$2"
  shift 2
  local rep
  for rep in $(seq 1 "$REPS"); do
    "$@" | grep -E '^(TIME|THREAD) ' | sed -E "s/^(TIME|THREAD) /\1 rep=$rep img=$img /" >> "$log"
  done
}

for img in lena fruit puj damma sulfur; do
  for ext in pgm ppm; do
    echo "secuencial $img.$ext"
    run "$RAW/d2_seq.txt" "$img.$ext" ./filterer "images/$img.$ext" "$OUT/${img}_seq.$ext"
  done
done

NODES=(node1 node2 node3 node4)
for img in damma sulfur; do
  for ext in pgm ppm; do
    echo "hilos $img.$ext"
    run "$RAW/d3_threads.txt" "$img.$ext" ./th_filterer "images/$img.$ext" "$OUT/${img}_th.$ext"
    for t in 1 2 4 8 16; do
      echo "openmp $t $img.$ext"
      run "$RAW/d3_omp.txt" "$img.$ext" env OMP_NUM_THREADS="$t" ./omp_filterer "images/$img.$ext" "$OUT/${img}_omp.$ext"
    done
    for n in 1 2 4; do
      hosts=$(IFS=,; echo "${NODES[*]:0:$n}")
      echo "mpi $n $img.$ext"
      run "$RAW/d4_mpi.txt" "$img.$ext" mpirun -np "$n" --host "$hosts" ./mpi_filterer "images/$img.$ext" "$OUT/${img}_mpi.$ext"
    done
  done
done

{
  echo "fecha: $(date -Iseconds)"
  echo "nucleos visibles en el contenedor: $(nproc)"
  echo "repeticiones: $REPS"
  lscpu | grep -E 'Model name|^CPU\(s\)|Thread|Core|Socket' || true
  g++ --version | head -1
  mpirun --version | head -1
} > results/env.txt

python3 scripts/summarize.py
