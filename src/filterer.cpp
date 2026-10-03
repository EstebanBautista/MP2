#include "convolver.h"
#include "pipeline.h"

static void sequential(const Filter& f, const Image& src, Image& dst) {
  Convolver::apply(f, src, dst);
}

int main(int argc, char* argv[]) { return runPipeline("sequential", 1, argc, argv, sequential); }
