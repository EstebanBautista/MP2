#include "omp_convolver.h"

#include <omp.h>

#include "convolver.h"
#include "image.h"

void OmpConvolver::apply(const Filter& f, const Image& src, Image& dst) {
  const int width = src.width();
  const int height = src.height();
#pragma omp parallel for schedule(static)
  for (int y = 0; y < height; ++y) {
    Convolver::applyRegion(f, src, dst, 0, width, y, y + 1);
  }
}

int OmpConvolver::maxThreads() { return omp_get_max_threads(); }
