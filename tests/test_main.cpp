#include <cstdio>

#include "test.h"

int g_checks = 0;
int g_failures = 0;

int main() {
  test_image();
  test_pnm_io();
  std::printf("%d checks, %d failures\n", g_checks, g_failures);
  return g_failures == 0 ? 0 : 1;
}
