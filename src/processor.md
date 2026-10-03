# processor — Diseño 1 (aplicación base)

Lee una imagen PGM (P2) o PPM (P3) y escribe una copia. Es la base orientada a objetos
sobre la que se construyen los demás diseños:

- `Image`: dimensiones, canales, valor máximo y píxeles en un arreglo `int*` (RGB intercalado).
- `PnmReader`: carga el archivo completo, salta comentarios `#` y valida encabezado y datos.
- `PnmWriter`: arma la salida en un buffer y la escribe de una sola vez.
- `Timer`: tiempo total (reloj de pared) y tiempo de CPU.

## Compilación (dentro del contenedor)

    make processor

## Uso

    ./processor images/lena.ppm output/lena2.ppm
    ./processor - output/lena2.ppm < images/lena.ppm    # lectura desde la entrada estándar
