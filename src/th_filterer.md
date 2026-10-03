# th_filterer — Diseño 3 (memoria compartida con hilos)

Divide la imagen en cuatro regiones (arriba-izquierda, arriba-derecha, abajo-izquierda,
abajo-derecha) con `midX = ancho/2` y `midY = alto/2`. Crea cuatro `std::thread`; cada uno
recibe su región y el filtro y llama a `Convolver::applyRegion`.

La imagen de origen es compartida y solo se lee; cada hilo escribe en una región distinta de
la imagen destino. Por eso no se necesitan mutex ni otra sincronización aparte del `join`.

Además de la línea `TIME`, imprime una línea `THREAD` por región con su tiempo de CPU.

## Compilación y uso (dentro del contenedor)

    make th_filterer
    ./th_filterer images/fruit.pgm output/fruit_blur2.pgm --f blur
    ./th_filterer images/damma.ppm output/damma_th.ppm
