#include "convolver.h"
#include "filters.h"
#include "image.h"
#include "test.h"

void test_convolver() {
  Image src(6, 4, 1, 255);
  for (long i = 0; i < src.size(); ++i) src.data()[i] = 50;
  Image dst = src.cloneEmpty();
  Convolver::applyRegion(*FilterRegistry::find("blur"), src, dst, 0, 3, 0, 4);
  bool leftDone = true, rightUntouched = true;
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 6; ++x) {
      if (x < 3) leftDone = leftDone && dst.at(x, y, 0) == 50;
      else rightUntouched = rightUntouched && dst.at(x, y, 0) == 0;
    }
  }
  CHECK(leftDone);
  CHECK(rightUntouched);

  Image rgb(5, 3, 3, 255);
  fillPattern(rgb);
  Image whole = rgb.cloneEmpty();
  Convolver::apply(*FilterRegistry::find("sharpen"), rgb, whole);
  Image parts = rgb.cloneEmpty();
  Convolver::applyRegion(*FilterRegistry::find("sharpen"), rgb, parts, 0, 5, 0, 1);
  Convolver::applyRegion(*FilterRegistry::find("sharpen"), rgb, parts, 0, 5, 1, 3);
  CHECK(sameImage(whole, parts));
}
