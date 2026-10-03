# MP2 — Manipulación de imágenes con programación paralela

Filtros de convolución 3×3 sobre imágenes PGM (P2, escala de grises) y PPM (P3, color),
en cuatro diseños: base, secuencial, memoria compartida (hilos y OpenMP) y memoria
distribuida (MPI entre contenedores Docker).

| Diseño | Rama | Ejecutable | Descripción |
|---|---|---|---|
| 1 | `diseno-1-base` | `processor` | Lectura y escritura PGM/PPM orientada a objetos |
| 2 | `diseno-2-secuencial` | `filterer` | Filtros en un solo hilo |
| 3 | `diseno-3-memoria-compartida` | `th_filterer`, `omp_filterer` | Cuatro hilos por cuadrantes; OpenMP por filas |
| 4 | `diseno-4-memoria-distribuida` | `mpi_filterer` | Franjas con halo entre cuatro contenedores |

Filtros: `blur`, `laplace`, `sharpen` (obligatorios) y `gaussian`, `sobel`.

## Requisitos

Docker Desktop. Todo se compila y ejecuta dentro de los contenedores.

## Puesta en marcha

    docker compose build
    docker compose up -d
    docker compose exec -u mpi node1 make all test

## Uso

    docker compose exec -u mpi node1 ./processor images/lena.ppm output/lena2.ppm
    docker compose exec -u mpi node1 ./filterer images/fruit.ppm output/fruit_blur.ppm --f blur
    docker compose exec -u mpi node1 ./th_filterer images/fruit.pgm output/fruit_blur2.pgm --f blur
    docker compose exec -u mpi node1 bash -c "OMP_NUM_THREADS=4 ./omp_filterer images/sulfur.pgm output/sulfur_N.pgm"
    docker compose exec -u mpi node1 mpirun -np 4 --host node1,node2,node3,node4 ./mpi_filterer images/sulfur.pgm output/sulfur_mpi.pgm

Opciones comunes: `<entrada> <salida> [--f <filtro>]...`. Sin `--f` se aplican blur, laplace
y sharpen. Con varios filtros se escribe `<salida>_<filtro>.<ext>`. `--f all` aplica los cinco.

## Verificación y mediciones

    docker compose exec -u mpi node1 bash scripts/verify.sh --mpi   # salidas idénticas a la secuencial
    docker compose exec -u mpi node1 bash scripts/bench.sh          # genera results/*.csv

Cada ejecución imprime líneas `TIME` con tiempo de lectura, filtrado (pared y CPU), escritura
y total. `results/summary.csv` resume speedup y eficiencia frente a la versión secuencial.

## Estructura

    headers/   clases (Image, PnmReader/Writer, Filter, Convolver, Timer, ...)
    src/       implementación y un main por diseño (con su .md)
    tests/     pruebas unitarias (make test)
    scripts/   verificación, mediciones y resumen
    results/   tiempos medidos
    docker/    imagen con g++, OpenMP y OpenMPI

## Formatos

- [Especificación PPM](http://netpbm.sourceforge.net/doc/ppm.html)
- [Especificación PGM](http://netpbm.sourceforge.net/doc/pgm.html)
- [Kernels de convolución](https://en.wikipedia.org/wiki/Kernel_(image_processing))
