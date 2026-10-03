#include "convolver.h"

#include "filter.h"
#include "image.h"

void Convolver::applyRegion(const Filter& f, const Image& src, Image& dst, int x0, int x1, int y0,
                            int y1) {
  const int channels = src.channels();
  for (int y = y0; y < y1; ++y) {
    for (int x = x0; x < x1; ++x) {
      for (int c = 0; c < channels; ++c) {
        dst.set(x, y, c, f.computePixel(src, x, y, c));
      }
    }
  }
}

void Convolver::apply(const Filter& f, const Image& src, Image& dst) {
  applyRegion(f, src, dst, 0, src.width(), 0, src.height());
}
