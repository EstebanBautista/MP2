#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <ctime>

class Timer {
 public:
  Timer();
  void start();
  double wallMs() const;
  double cpuMs() const;
  static double threadCpuMs();

 private:
  std::chrono::steady_clock::time_point wall0_;
  std::clock_t cpu0_;
};

struct TimeReport {
  const char* design;
  const char* image;
  const char* filter;
  int threads;
  int nodes;
  int rank;
  double readMs;
  double commMs;
  double filterWallMs;
  double filterCpuMs;
  double writeMs;
  double totalWallMs;
  double totalCpuMs;
};

void printTime(const TimeReport& r);
const char* baseName(const char* path);

#endif
