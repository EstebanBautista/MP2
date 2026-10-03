#ifndef TEST_H
#define TEST_H

#include <cstdio>

extern int g_checks;
extern int g_failures;

#define CHECK(cond)                                                          \
  do {                                                                       \
    ++g_checks;                                                              \
    if (!(cond)) {                                                           \
      ++g_failures;                                                          \
      std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);   \
    }                                                                        \
  } while (0)

#include <cstring>

#include "image.h"

void test_image();
void test_pnm_io();
void test_timer();
void test_filters();
void test_convolver();
void test_cli();

inline void fillPattern(Image& img) {
  const long n = img.size();
  int* d = img.data();
  for (long i = 0; i < n; ++i) d[i] = (int)((i * 37 + 11) % (img.maxval() + 1));
}

inline bool sameImage(const Image& a, const Image& b) {
  return a.width() == b.width() && a.height() == b.height() &&
         a.channels() == b.channels() && a.maxval() == b.maxval() &&
         std::memcmp(a.data(), b.data(), sizeof(int) * (size_t)a.size()) == 0;
}

#endif
