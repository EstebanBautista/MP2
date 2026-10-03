#ifndef CLI_H
#define CLI_H

class Filter;

const int kMaxFilters = 8;

struct CliOptions {
  const char* input;
  const char* output;
  const Filter* filters[kMaxFilters];
  int filterCount;
  bool singleOutput;
};

bool parseCli(int argc, char* argv[], CliOptions& opts, char* err, int errLen);
void outputPathFor(const char* output, const char* filterName, char* buf, int bufLen);
void printUsage(const char* prog);

#endif
