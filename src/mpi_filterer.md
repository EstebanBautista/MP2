# mpi_filterer — Diseño 4 (memoria distribuida con MPI)

Cuatro contenedores Docker (`node1`..`node4`) forman el clúster; `mpirun` lanza un proceso
por contenedor a través de SSH.

1. Rank 0 lee la imagen y difunde ancho, alto, canales y maxval con `MPI_Bcast`.
2. La imagen se divide en franjas horizontales de filas (`computeStrips`). Rank 0 envía a cada
   nodo su franja más una fila de halo arriba y abajo (`MPI_Send`/`MPI_Recv`); el halo permite
   aplicar el kernel 3×3 en los bordes de la franja sin pedir datos a los vecinos.
3. Cada nodo aplica los filtros a su franja con `Convolver::applyRegion`.
4. Rank 0 recoge las franjas filtradas con `MPI_Gatherv` y escribe una salida por filtro.
5. Cada nodo mide su CPU y su tiempo de pared (filtrado y comunicación); rank 0 los recoge con
   `MPI_Gather` e imprime una línea `TIME` por nodo y filtro.
6. Al final, cada nodo reporta su tiempo de ejecución total (desde antes de `MPI_Init` hasta el
   final) y rank 0 imprime una línea `TOTAL` por nodo.

## Uso

    docker compose up -d
    docker compose exec -u mpi node1 make mpi_filterer
    docker compose exec -u mpi node1 mpirun -np 4 --host node1,node2,node3,node4 \
        ./mpi_filterer images/sulfur.pgm output/sulfur_mpi.pgm
