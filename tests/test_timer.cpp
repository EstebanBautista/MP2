#include <cstring>

#include "test.h"
#include "timer.h"

void test_timer() {
  Timer t;
  volatile double acc = 0;
  for (int i = 0; i < 2000000; ++i) acc = acc + i * 0.5;
  CHECK(t.wallMs() >= 0.0);
  CHECK(t.cpuMs() >= 0.0);
  CHECK(Timer::threadCpuMs() > 0.0);

  CHECK(std::strcmp(baseName("images/lena.pgm"), "lena.pgm") == 0);
  CHECK(std::strcmp(baseName("lena.pgm"), "lena.pgm") == 0);
  CHECK(std::strcmp(baseName("C:\\x\\lena.ppm"), "lena.ppm") == 0);
  CHECK(std::strcmp(baseName("-"), "stdin") == 0);
}
