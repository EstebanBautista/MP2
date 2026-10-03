#!/usr/bin/env bash
# Compara las salidas paralelas contra la secuencial, byte a byte.
# Uso: scripts/verify.sh [--mpi]
set -euo pipefail
cd "$(dirname "$0")/.."

WITH_MPI=0
if [ "${1:-}" = "--mpi" ]; then WITH_MPI=1; fi

OUT=output/verify
mkdir -p "$OUT"
make -s all

fail=0
check() {
  if cmp -s "$1" "$2"; then
    echo "OK   $2"
  else
    echo "DIFF $2 (referencia $1)"
    fail=1
  fi
}

for img in feep lena fruit damma sulfur; do
  for ext in pgm ppm; do
    src="images/$img.$ext"
    if [ ! -f "$src" ]; then continue; fi
    ./filterer "$src" "$OUT/${img}_seq.$ext" --f all > /dev/null
    ./th_filterer "$src" "$OUT/${img}_th.$ext" --f all > /dev/null
    OMP_NUM_THREADS=4 ./omp_filterer "$src" "$OUT/${img}_omp.$ext" --f all > /dev/null
    if [ "$WITH_MPI" = 1 ]; then
      mpirun -np 4 --host node1,node2,node3,node4 ./mpi_filterer "$src" "$OUT/${img}_mpi.$ext" --f all > /dev/null
    fi
    for f in blur laplace sharpen gaussian sobel; do
      ref="$OUT/${img}_seq_$f.$ext"
      check "$ref" "$OUT/${img}_th_$f.$ext"
      check "$ref" "$OUT/${img}_omp_$f.$ext"
      if [ "$WITH_MPI" = 1 ]; then check "$ref" "$OUT/${img}_mpi_$f.$ext"; fi
    done
  done
done

if [ "$fail" = 0 ]; then
  echo "VERIFY OK"
else
  echo "VERIFY FAILED"
  exit 1
fi
