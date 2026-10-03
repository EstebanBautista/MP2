#include "timer.h"

#include <cstdio>
#include <cstring>
#include <time.h>

Timer::Timer() { start(); }

void Timer::start() {
  wall0_ = std::chrono::steady_clock::now();
  cpu0_ = std::clock();
}

double Timer::wallMs() const {
  return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - wall0_)
      .count();
}

double Timer::cpuMs() const {
  return 1000.0 * (double)(std::clock() - cpu0_) / CLOCKS_PER_SEC;
}

double Timer::threadCpuMs() {
  struct timespec ts;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
  return ts.tv_sec * 1000.0 + ts.tv_nsec / 1.0e6;
}

void printTime(const TimeReport& r) {
  std::printf(
      "TIME design=%s image=%s filter=%s threads=%d nodes=%d rank=%d read_ms=%.3f "
      "comm_ms=%.3f filter_wall_ms=%.3f filter_cpu_ms=%.3f write_ms=%.3f "
      "total_wall_ms=%.3f total_cpu_ms=%.3f\n",
      r.design, r.image, r.filter, r.threads, r.nodes, r.rank, r.readMs, r.commMs,
      r.filterWallMs, r.filterCpuMs, r.writeMs, r.totalWallMs, r.totalCpuMs);
  std::fflush(stdout);
}

const char* baseName(const char* path) {
  if (std::strcmp(path, "-") == 0) return "stdin";
  const char* slash = std::strrchr(path, '/');
  const char* back = std::strrchr(path, '\\');
  const char* last = slash;
  if (back != nullptr && (last == nullptr || back > last)) last = back;
  return last != nullptr ? last + 1 : path;
}
