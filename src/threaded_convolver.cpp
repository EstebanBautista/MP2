#include "threaded_convolver.h"

#include <thread>

#include "convolver.h"
#include "image.h"
#include "timer.h"

void ThreadedConvolver::quadrant(int index, int width, int height, int& x0, int& x1, int& y0,
                                 int& y1) {
  const int midX = width / 2;
  const int midY = height / 2;
  const bool right = index == 1 || index == 3;
  const bool bottom = index == 2 || index == 3;
  x0 = right ? midX : 0;
  x1 = right ? width : midX;
  y0 = bottom ? midY : 0;
  y1 = bottom ? height : midY;
}

void ThreadedConvolver::applyQuadrants(const Filter& f, const Image& src, Image& dst,
                                       double threadCpuMs[kRegions]) {
  std::thread workers[kRegions];
  for (int i = 0; i < kRegions; ++i) {
    workers[i] = std::thread([&f, &src, &dst, threadCpuMs, i]() {
      const double cpu0 = Timer::threadCpuMs();
      int x0, x1, y0, y1;
      quadrant(i, src.width(), src.height(), x0, x1, y0, y1);
      Convolver::applyRegion(f, src, dst, x0, x1, y0, y1);
      threadCpuMs[i] = Timer::threadCpuMs() - cpu0;
    });
  }
  for (int i = 0; i < kRegions; ++i) workers[i].join();
}
