#include "filters.h"

#include <cmath>
#include <cstring>

#include "image.h"

namespace {

int clampInt(long value, long low, long high) {
  return (int)(value < low ? low : (value > high ? high : value));
}

int sampleClamped(const Image& src, int x, int y, int c) {
  const int sx = x < 0 ? 0 : (x >= src.width() ? src.width() - 1 : x);
  const int sy = y < 0 ? 0 : (y >= src.height() ? src.height() - 1 : y);
  return src.at(sx, sy, c);
}

int convolve3x3(const int kernel[3][3], const Image& src, int x, int y, int c) {
  int sum = 0;
  for (int ky = -1; ky <= 1; ++ky) {
    for (int kx = -1; kx <= 1; ++kx) {
      sum += kernel[ky + 1][kx + 1] * sampleClamped(src, x + kx, y + ky, c);
    }
  }
  return sum;
}

const int K_BLUR[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
const int K_LAPLACE[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
const int K_SHARPEN[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
const int K_GAUSSIAN[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
const int K_SOBEL_X[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
const int K_SOBEL_Y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

const KernelFilter BLUR("blur", K_BLUR, 9);
const KernelFilter LAPLACE("laplace", K_LAPLACE, 1);
const KernelFilter SHARPEN("sharpen", K_SHARPEN, 1);
const KernelFilter GAUSSIAN("gaussian", K_GAUSSIAN, 16);
const SobelFilter SOBEL{};

const Filter* const REGISTRY[] = {&BLUR, &LAPLACE, &SHARPEN, &GAUSSIAN, &SOBEL};
const int REGISTRY_SIZE = (int)(sizeof(REGISTRY) / sizeof(REGISTRY[0]));

}  // namespace

KernelFilter::KernelFilter(const char* name, const int kernel[3][3], int divisor)
    : name_(name), divisor_(divisor) {
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) kernel_[i][j] = kernel[i][j];
}

int KernelFilter::computePixel(const Image& src, int x, int y, int c) const {
  const int sum = convolve3x3(kernel_, src, x, y, c);
  const long result = std::lround((double)sum / divisor_);
  return clampInt(result, 0, src.maxval());
}

int SobelFilter::computePixel(const Image& src, int x, int y, int c) const {
  const double gx = convolve3x3(K_SOBEL_X, src, x, y, c);
  const double gy = convolve3x3(K_SOBEL_Y, src, x, y, c);
  return clampInt(std::lround(std::sqrt(gx * gx + gy * gy)), 0, src.maxval());
}

const Filter* FilterRegistry::find(const char* name) {
  for (int i = 0; i < REGISTRY_SIZE; ++i) {
    if (std::strcmp(REGISTRY[i]->name(), name) == 0) return REGISTRY[i];
  }
  return nullptr;
}

int FilterRegistry::count() { return REGISTRY_SIZE; }

const Filter* FilterRegistry::at(int index) {
  return (index >= 0 && index < REGISTRY_SIZE) ? REGISTRY[index] : nullptr;
}
