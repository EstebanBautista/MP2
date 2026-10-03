#include <cstring>

#include "filters.h"
#include "image.h"
#include "test.h"

static int applyAt(const char* name, const Image& src, int x, int y, int c) {
  return FilterRegistry::find(name)->computePixel(src, x, y, c);
}

static Image ramp3x3() {
  Image img(3, 3, 1, 255);
  for (int i = 0; i < 9; ++i) img.data()[i] = i;
  return img;
}

void test_filters() {
  CHECK(FilterRegistry::count() == 5);
  CHECK(FilterRegistry::mandatoryCount() == 3);
  CHECK(std::strcmp(FilterRegistry::at(0)->name(), "blur") == 0);
  CHECK(std::strcmp(FilterRegistry::at(1)->name(), "laplace") == 0);
  CHECK(std::strcmp(FilterRegistry::at(2)->name(), "sharpen") == 0);
  CHECK(std::strcmp(FilterRegistry::at(3)->name(), "gaussian") == 0);
  CHECK(std::strcmp(FilterRegistry::at(4)->name(), "sobel") == 0);
  CHECK(FilterRegistry::find("nope") == nullptr);

  Image uniform(4, 4, 1, 255);
  for (long i = 0; i < uniform.size(); ++i) uniform.data()[i] = 100;
  CHECK(applyAt("blur", uniform, 1, 1, 0) == 100);
  CHECK(applyAt("gaussian", uniform, 0, 3, 0) == 100);
  CHECK(applyAt("sharpen", uniform, 3, 0, 0) == 100);
  CHECK(applyAt("laplace", uniform, 2, 2, 0) == 0);
  CHECK(applyAt("sobel", uniform, 0, 0, 0) == 0);

  Image ramp = ramp3x3();
  CHECK(applyAt("blur", ramp, 1, 1, 0) == 4);
  CHECK(applyAt("gaussian", ramp, 1, 1, 0) == 4);

  Image spot(3, 3, 1, 255);
  spot.set(1, 1, 0, 10);
  CHECK(applyAt("laplace", spot, 1, 1, 0) == 80);
  CHECK(applyAt("laplace", spot, 0, 0, 0) == 0);
  CHECK(applyAt("sharpen", spot, 1, 1, 0) == 50);

  Image bright(3, 3, 1, 255);
  bright.set(1, 1, 0, 200);
  CHECK(applyAt("laplace", bright, 1, 1, 0) == 255);

  Image edge(3, 3, 1, 255);
  for (int x = 0; x < 3; ++x) edge.set(x, 2, 0, 10);
  CHECK(applyAt("sobel", edge, 1, 1, 0) == 40);

  Image single(1, 1, 1, 255);
  single.set(0, 0, 0, 7);
  CHECK(applyAt("blur", single, 0, 0, 0) == 7);
  CHECK(applyAt("sharpen", single, 0, 0, 0) == 7);
  CHECK(applyAt("laplace", single, 0, 0, 0) == 0);

  Image rgb(1, 1, 3, 255);
  rgb.set(0, 0, 0, 10);
  rgb.set(0, 0, 1, 20);
  rgb.set(0, 0, 2, 30);
  CHECK(applyAt("blur", rgb, 0, 0, 0) == 10);
  CHECK(applyAt("blur", rgb, 0, 0, 1) == 20);
  CHECK(applyAt("blur", rgb, 0, 0, 2) == 30);
}
