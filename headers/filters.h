#ifndef FILTERS_H
#define FILTERS_H

#include "filter.h"

class KernelFilter : public Filter {
 public:
  KernelFilter(const char* name, const int kernel[3][3], int divisor);
  const char* name() const override { return name_; }
  int computePixel(const Image& src, int x, int y, int c) const override;

 private:
  const char* name_;
  int kernel_[3][3];
  int divisor_;
};

class SobelFilter : public Filter {
 public:
  const char* name() const override { return "sobel"; }
  int computePixel(const Image& src, int x, int y, int c) const override;
};

class FilterRegistry {
 public:
  static const Filter* find(const char* name);
  static int count();
  static const Filter* at(int index);
  static int mandatoryCount() { return 3; }
};

#endif
