#include <cstdio>

#include "filter.h"
#include "image.h"
#include "pipeline.h"
#include "threaded_convolver.h"

static const char* const kRegionNames[ThreadedConvolver::kRegions] = {
    "arriba-izquierda", "arriba-derecha", "abajo-izquierda", "abajo-derecha"};

static double g_threadCpuMs[ThreadedConvolver::kRegions];

static void quadrants(const Filter& f, const Image& src, Image& dst) {
  ThreadedConvolver::applyQuadrants(f, src, dst, g_threadCpuMs);
}

static void reportThreads(const Filter& f, const Image& src) {
  for (int i = 0; i < ThreadedConvolver::kRegions; ++i) {
    int x0, x1, y0, y1;
    ThreadedConvolver::quadrant(i, src.width(), src.height(), x0, x1, y0, y1);
    std::printf("THREAD filter=%s region=%s x0=%d x1=%d y0=%d y1=%d cpu_ms=%.3f\n", f.name(),
                kRegionNames[i], x0, x1, y0, y1, g_threadCpuMs[i]);
  }
}

int main(int argc, char* argv[]) {
  return runPipeline("threads", ThreadedConvolver::kRegions, argc, argv, quadrants,
                     reportThreads);
}
