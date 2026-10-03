#include "strips.h"

#include <cstring>

void computeStrips(int height, int parts, int* y0, int* y1) {
  const int base = height / parts;
  const int extra = height % parts;
  int start = 0;
  for (int i = 0; i < parts; ++i) {
    const int rows = base + (i < extra ? 1 : 0);
    y0[i] = start;
    y1[i] = start + rows;
    start += rows;
  }
}

void haloBounds(int height, int y0, int y1, int& h0, int& h1) {
  if (y0 == y1) {
    h0 = h1 = y0;
    return;
  }
  h0 = y0 > 0 ? y0 - 1 : 0;
  h1 = y1 < height ? y1 + 1 : height;
}

Image copyRows(const Image& src, int y0, int y1) {
  Image out(src.width(), y1 - y0, src.channels(), src.maxval());
  const long rowInts = (long)src.width() * src.channels();
  if (y1 > y0) {
    std::memcpy(out.data(), src.data() + y0 * rowInts, sizeof(int) * (size_t)((y1 - y0) * rowInts));
  }
  return out;
}
