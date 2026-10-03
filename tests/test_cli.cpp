#include <cstring>

#include "cli.h"
#include "filter.h"
#include "test.h"

static bool parseArgs(int argc, const char* const* args, CliOptions& opts, char* err) {
  char* argv[16];
  for (int i = 0; i < argc; ++i) argv[i] = const_cast<char*>(args[i]);
  return parseCli(argc, argv, opts, err, 256);
}

void test_cli() {
  char err[256];
  CliOptions o;

  const char* defaults[] = {"p", "in.pgm", "out.pgm"};
  CHECK(parseArgs(3, defaults, o, err));
  CHECK(std::strcmp(o.input, "in.pgm") == 0 && std::strcmp(o.output, "out.pgm") == 0);
  CHECK(o.filterCount == 3 && !o.singleOutput);
  CHECK(std::strcmp(o.filters[0]->name(), "blur") == 0);
  CHECK(std::strcmp(o.filters[2]->name(), "sharpen") == 0);

  const char* one[] = {"p", "in.pgm", "out.pgm", "--f", "blur"};
  CHECK(parseArgs(5, one, o, err));
  CHECK(o.filterCount == 1 && o.singleOutput);

  const char* flagFirst[] = {"p", "--f", "laplace", "in.pgm", "out.pgm"};
  CHECK(parseArgs(5, flagFirst, o, err));
  CHECK(o.filterCount == 1 && std::strcmp(o.filters[0]->name(), "laplace") == 0);

  const char* all[] = {"p", "in.pgm", "out.pgm", "--f", "all"};
  CHECK(parseArgs(5, all, o, err));
  CHECK(o.filterCount == 5 && !o.singleOutput);

  const char* dup[] = {"p", "in.pgm", "out.pgm", "--f", "blur", "--f", "blur"};
  CHECK(parseArgs(7, dup, o, err));
  CHECK(o.filterCount == 1);

  const char* unknown[] = {"p", "in.pgm", "out.pgm", "--f", "nope"};
  CHECK(!parseArgs(5, unknown, o, err));
  CHECK(std::strstr(err, "nope") != nullptr);

  const char* missingName[] = {"p", "in.pgm", "out.pgm", "--f"};
  CHECK(!parseArgs(4, missingName, o, err));

  const char* missingPaths[] = {"p", "in.pgm"};
  CHECK(!parseArgs(2, missingPaths, o, err));

  const char* extra[] = {"p", "a", "b", "c"};
  CHECK(!parseArgs(4, extra, o, err));

  char path[256];
  outputPathFor("out/sulfur_N.pgm", "blur", path, sizeof path);
  CHECK(std::strcmp(path, "out/sulfur_N_blur.pgm") == 0);
  outputPathFor("out/a", "blur", path, sizeof path);
  CHECK(std::strcmp(path, "out/a_blur") == 0);
  outputPathFor("./x.y/out", "sobel", path, sizeof path);
  CHECK(std::strcmp(path, "./x.y/out_sobel") == 0);
}
