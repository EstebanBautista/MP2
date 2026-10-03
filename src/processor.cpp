#include <cstdio>

#include "image.h"
#include "pnm_io.h"
#include "timer.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::fprintf(stderr, "uso: %s <entrada.pgm|entrada.ppm|-> <salida>\n", argv[0]);
    std::fprintf(stderr, "  use '-' como entrada para leer desde la entrada estandar\n");
    return 1;
  }
  char err[256];
  Image img;

  Timer t;
  if (!PnmReader::read(argv[1], img, err, sizeof err)) {
    std::fprintf(stderr, "error: %s\n", err);
    return 2;
  }
  const double readMs = t.wallMs();
  const double readCpu = t.cpuMs();

  t.start();
  if (!PnmWriter::write(argv[2], img, err, sizeof err)) {
    std::fprintf(stderr, "error: %s\n", err);
    return 3;
  }
  const double writeMs = t.wallMs();
  const double writeCpu = t.cpuMs();

  TimeReport r = {};
  r.design = "processor";
  r.image = baseName(argv[1]);
  r.filter = "none";
  r.threads = 1;
  r.nodes = 1;
  r.readMs = readMs;
  r.writeMs = writeMs;
  r.totalWallMs = readMs + writeMs;
  r.totalCpuMs = readCpu + writeCpu;
  printTime(r);
  return 0;
}
