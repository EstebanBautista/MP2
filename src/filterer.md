# filterer — Diseño 2 (versión secuencial)

Aplica filtros de convolución 3×3 a imágenes PGM/PPM en un solo hilo.

- `Filter` (clase abstracta) define `computePixel`; `KernelFilter` cubre blur, laplace,
  sharpen y gaussian; `SobelFilter` calcula la magnitud del gradiente.
- `FilterRegistry` lista los filtros disponibles: agregar uno nuevo es una línea.
- `Convolver::applyRegion` recorre una región de la imagen; los diseños paralelos reutilizan
  esta misma función, así que el resultado es idéntico en todas las versiones.
- Bordes por repetición del píxel del borde (clamp-to-edge).

## Compilación (dentro del contenedor)

    make filterer

## Uso

    ./filterer images/fruit.ppm output/fruit_blur.ppm --f blur     # un filtro
    ./filterer images/lena.pgm output/lena.pgm                     # blur, laplace y sharpen
    ./filterer images/lena.pgm output/lena.pgm --f all             # los cinco filtros

Con varios filtros se escribe `<salida>_<filtro>.<ext>`. Cada filtro imprime una línea `TIME`
con tiempos de lectura, filtrado (pared y CPU) y escritura; su `total_*` es lectura + filtrado +
escritura de ese filtro. Al final, una línea `TOTAL` da el tiempo de ejecución completo del
programa (pared y CPU), medido de principio a fin una sola vez.
