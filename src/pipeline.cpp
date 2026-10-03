#include "pipeline.h"

#include <cstdio>
#include <new>

#include "cli.h"
#include "filter.h"
#include "image.h"
#include "pnm_io.h"
#include "timer.h"

int runPipeline(const char* design, int threads, int argc, char* argv[], FilterStrategy strategy) {
  Timer total;
  CliOptions opts;
  char err[256];
  if (!parseCli(argc, argv, opts, err, sizeof err)) {
    std::fprintf(stderr, "error: %s\n", err);
    printUsage(argv[0]);
    return 1;
  }

  Timer t;
  Image src;
  if (!PnmReader::read(opts.input, src, err, sizeof err)) {
    std::fprintf(stderr, "error: %s\n", err);
    return 2;
  }
  const double readMs = t.wallMs();
  const double readCpu = t.cpuMs();

  for (int i = 0; i < opts.filterCount; ++i) {
    const Filter& f = *opts.filters[i];
    Image dst;
    try {
      dst = src.cloneEmpty();
    } catch (const std::bad_alloc&) {
      std::fprintf(stderr, "error: memoria insuficiente para la imagen de salida\n");
      return 2;
    }

    t.start();
    strategy(f, src, dst);
    const double filterWall = t.wallMs();
    const double filterCpu = t.cpuMs();

    char path[1024];
    if (opts.singleOutput) {
      std::snprintf(path, sizeof path, "%s", opts.output);
    } else {
      outputPathFor(opts.output, f.name(), path, sizeof path);
    }

    t.start();
    if (!PnmWriter::write(path, dst, err, sizeof err)) {
      std::fprintf(stderr, "error: %s\n", err);
      return 3;
    }
    const double writeMs = t.wallMs();
    const double writeCpu = t.cpuMs();

    TimeReport r = {};
    r.design = design;
    r.image = baseName(opts.input);
    r.filter = f.name();
    r.threads = threads;
    r.nodes = 1;
    r.readMs = readMs;
    r.filterWallMs = filterWall;
    r.filterCpuMs = filterCpu;
    r.writeMs = writeMs;
    r.totalWallMs = readMs + filterWall + writeMs;
    r.totalCpuMs = readCpu + filterCpu + writeCpu;
    printTime(r);
  }
  printTotal(design, baseName(opts.input), threads, 1, 0, opts.filterCount, total.wallMs(),
             total.cpuMs());
  return 0;
}
