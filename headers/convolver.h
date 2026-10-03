#ifndef CONVOLVER_H
#define CONVOLVER_H

class Filter;
class Image;

class Convolver {
 public:
  static void applyRegion(const Filter& f, const Image& src, Image& dst, int x0, int x1, int y0,
                          int y1);
  static void apply(const Filter& f, const Image& src, Image& dst);
};

#endif
