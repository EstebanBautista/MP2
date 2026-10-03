#include "omp_convolver.h"
#include "pipeline.h"

static void openmp(const Filter& f, const Image& src, Image& dst) { OmpConvolver::apply(f, src, dst); }

int main(int argc, char* argv[]) {
  return runPipeline("openmp", OmpConvolver::maxThreads(), argc, argv, openmp);
}
