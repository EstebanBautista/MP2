#ifndef OMP_CONVOLVER_H
#define OMP_CONVOLVER_H

class Filter;
class Image;

class OmpConvolver {
 public:
  static void apply(const Filter& f, const Image& src, Image& dst);
  static int maxThreads();
};

#endif
