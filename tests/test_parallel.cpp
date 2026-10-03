#include "convolver.h"
#include "filters.h"
#include "image.h"
#include "omp_convolver.h"
#include "test.h"
#include "threaded_convolver.h"

static void checkQuadrantsCover(int w, int h) {
  int covered[64 * 64] = {0};
  for (int i = 0; i < ThreadedConvolver::kRegions; ++i) {
    int x0, x1, y0, y1;
    ThreadedConvolver::quadrant(i, w, h, x0, x1, y0, y1);
    for (int y = y0; y < y1; ++y)
      for (int x = x0; x < x1; ++x) ++covered[y * w + x];
  }
  bool exactlyOnce = true;
  for (int i = 0; i < w * h; ++i) exactlyOnce = exactlyOnce && covered[i] == 1;
  CHECK(exactlyOnce);
}

static void checkSameAsSequential(int w, int h, int channels) {
  Image src(w, h, channels, 255);
  fillPattern(src);
  for (int k = 0; k < FilterRegistry::count(); ++k) {
    const Filter& f = *FilterRegistry::at(k);
    Image seq = src.cloneEmpty();
    Convolver::apply(f, src, seq);

    Image th = src.cloneEmpty();
    double cpu[ThreadedConvolver::kRegions];
    ThreadedConvolver::applyQuadrants(f, src, th, cpu);
    CHECK(sameImage(seq, th));

    Image omp = src.cloneEmpty();
    OmpConvolver::apply(f, src, omp);
    CHECK(sameImage(seq, omp));
  }
}

void test_parallel() {
  checkQuadrantsCover(37, 23);
  checkQuadrantsCover(1, 1);
  checkQuadrantsCover(1, 5);
  checkQuadrantsCover(64, 64);

  int x0, x1, y0, y1;
  ThreadedConvolver::quadrant(1, 10, 6, x0, x1, y0, y1);
  CHECK(x0 == 5 && x1 == 10 && y0 == 0 && y1 == 3);
  ThreadedConvolver::quadrant(2, 10, 6, x0, x1, y0, y1);
  CHECK(x0 == 0 && x1 == 5 && y0 == 3 && y1 == 6);

  checkSameAsSequential(37, 23, 1);
  checkSameAsSequential(37, 23, 3);
  checkSameAsSequential(1, 1, 1);
  checkSameAsSequential(1, 5, 3);
  checkSameAsSequential(64, 64, 3);
}
