#ifndef FILTER_H
#define FILTER_H

class Image;

class Filter {
 public:
  virtual ~Filter() {}
  virtual const char* name() const = 0;
  virtual int computePixel(const Image& src, int x, int y, int c) const = 0;
};

#endif
