# omp_filterer — Diseño 3 (memoria compartida con OpenMP)

Paraleliza el recorrido por filas con `#pragma omp parallel for schedule(static)`: cada hilo
recibe un bloque contiguo de filas y llama a `Convolver::applyRegion` fila por fila. El número
de hilos se controla con la variable de entorno `OMP_NUM_THREADS`.

Sin `--f` aplica los tres filtros (blur, laplace, sharpen) y escribe `<salida>_<filtro>.<ext>`.

## Compilación y uso (dentro del contenedor)

    make omp_filterer
    OMP_NUM_THREADS=4 ./omp_filterer images/sulfur.pgm output/sulfur_N.pgm
