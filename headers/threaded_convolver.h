#ifndef THREADED_CONVOLVER_H
#define THREADED_CONVOLVER_H

class Filter;
class Image;

class ThreadedConvolver {
 public:
  static const int kRegions = 4;

  static void quadrant(int index, int width, int height, int& x0, int& x1, int& y0, int& y1);
  static void applyQuadrants(const Filter& f, const Image& src, Image& dst,
                             double threadCpuMs[kRegions]);
};

#endif
