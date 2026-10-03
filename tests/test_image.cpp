#include <cstring>
#include <utility>

#include "image.h"
#include "test.h"

void test_image() {
  Image none;
  CHECK(none.empty());
  CHECK(none.size() == 0);

  Image img(4, 3, 3, 255);
  CHECK(img.width() == 4);
  CHECK(img.height() == 3);
  CHECK(img.channels() == 3);
  CHECK(img.maxval() == 255);
  CHECK(img.size() == 36);
  CHECK(std::strcmp(img.magic(), "P3") == 0);
  bool allZero = true;
  for (long i = 0; i < img.size(); ++i) allZero = allZero && img.data()[i] == 0;
  CHECK(allZero);

  img.set(3, 2, 2, 99);
  CHECK(img.at(3, 2, 2) == 99);
  CHECK(img.data()[35] == 99);

  Image gray(2, 2, 1, 15);
  CHECK(std::strcmp(gray.magic(), "P2") == 0);

  Image moved(std::move(img));
  CHECK(moved.at(3, 2, 2) == 99);
  CHECK(img.empty());

  Image assigned;
  assigned = std::move(moved);
  CHECK(assigned.width() == 4);
  CHECK(moved.empty());

  Image blank = assigned.cloneEmpty();
  CHECK(blank.width() == 4 && blank.height() == 3 && blank.channels() == 3);
  CHECK(blank.maxval() == 255);
  CHECK(blank.at(3, 2, 2) == 0);
}
