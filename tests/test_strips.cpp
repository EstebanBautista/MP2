#include <cstring>

#include "convolver.h"
#include "filters.h"
#include "image.h"
#include "strips.h"
#include "test.h"

static void checkCover(int height, int parts) {
  int y0[16], y1[16];
  computeStrips(height, parts, y0, y1);
  CHECK(y0[0] == 0);
  CHECK(y1[parts - 1] == height);
  bool ok = true;
  for (int i = 0; i < parts; ++i) {
    const int rows = y1[i] - y0[i];
    ok = ok && rows >= height / parts && rows <= height / parts + 1;
    if (i > 0) ok = ok && y0[i] == y1[i - 1];
  }
  CHECK(ok);
}

static void checkStripsMatchSequential(int w, int h, int channels, int parts) {
  Image src(w, h, channels, 255);
  fillPattern(src);
  int y0[16], y1[16];
  computeStrips(h, parts, y0, y1);
  const long rowInts = (long)w * channels;

  for (int k = 0; k < FilterRegistry::count(); ++k) {
    const Filter& f = *FilterRegistry::at(k);
    Image seq = src.cloneEmpty();
    Convolver::apply(f, src, seq);

    bool same = true;
    for (int p = 0; p < parts; ++p) {
      int h0, h1;
      haloBounds(h, y0[p], y1[p], h0, h1);
      Image local = copyRows(src, h0, h1);
      Image localDst = local.cloneEmpty();
      Convolver::applyRegion(f, local, localDst, 0, w, y0[p] - h0, y1[p] - h0);
      const int rows = y1[p] - y0[p];
      if (rows > 0) {
        same = same && std::memcmp(localDst.data() + (y0[p] - h0) * rowInts,
                                   seq.data() + y0[p] * rowInts,
                                   sizeof(int) * (size_t)(rows * rowInts)) == 0;
      }
    }
    CHECK(same);
  }
}

void test_strips() {
  const int heights[] = {1, 2, 7, 23, 1278};
  for (int hi = 0; hi < 5; ++hi)
    for (int parts = 1; parts <= 5; ++parts) checkCover(heights[hi], parts);

  int y0[4], y1[4];
  computeStrips(10, 4, y0, y1);
  CHECK(y0[0] == 0 && y1[0] == 3 && y0[1] == 3 && y1[1] == 6);
  CHECK(y0[2] == 6 && y1[2] == 8 && y0[3] == 8 && y1[3] == 10);

  int h0, h1;
  haloBounds(10, 0, 3, h0, h1);
  CHECK(h0 == 0 && h1 == 4);
  haloBounds(10, 3, 6, h0, h1);
  CHECK(h0 == 2 && h1 == 7);
  haloBounds(10, 8, 10, h0, h1);
  CHECK(h0 == 7 && h1 == 10);
  haloBounds(10, 4, 4, h0, h1);
  CHECK(h0 == 4 && h1 == 4);

  Image src(3, 4, 1, 255);
  fillPattern(src);
  Image rows = copyRows(src, 1, 3);
  CHECK(rows.width() == 3 && rows.height() == 2 && rows.maxval() == 255);
  CHECK(rows.at(0, 0, 0) == src.at(0, 1, 0) && rows.at(2, 1, 0) == src.at(2, 2, 0));

  checkStripsMatchSequential(13, 11, 3, 4);
  checkStripsMatchSequential(5, 7, 1, 4);
  checkStripsMatchSequential(4, 3, 1, 5);
  checkStripsMatchSequential(1, 1, 3, 4);
}
