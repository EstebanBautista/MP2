#include "cli.h"

#include <cstdio>
#include <cstring>

#include "filters.h"

namespace {

void addUnique(CliOptions& opts, const Filter* f) {
  for (int i = 0; i < opts.filterCount; ++i) {
    if (opts.filters[i] == f) return;
  }
  if (opts.filterCount < kMaxFilters) opts.filters[opts.filterCount++] = f;
}

}  // namespace

bool parseCli(int argc, char* argv[], CliOptions& opts, char* err, int errLen) {
  opts.input = nullptr;
  opts.output = nullptr;
  opts.filterCount = 0;
  opts.singleOutput = false;
  int positional = 0;

  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--f") == 0) {
      if (i + 1 >= argc) {
        std::snprintf(err, errLen, "falta el nombre del filtro despues de --f");
        return false;
      }
      const char* name = argv[++i];
      if (std::strcmp(name, "all") == 0) {
        for (int k = 0; k < FilterRegistry::count(); ++k) addUnique(opts, FilterRegistry::at(k));
      } else {
        const Filter* f = FilterRegistry::find(name);
        if (f == nullptr) {
          std::snprintf(err, errLen, "filtro desconocido: '%s'", name);
          return false;
        }
        addUnique(opts, f);
      }
    } else if (positional == 0) {
      opts.input = argv[i];
      ++positional;
    } else if (positional == 1) {
      opts.output = argv[i];
      ++positional;
    } else {
      std::snprintf(err, errLen, "argumento inesperado: '%s'", argv[i]);
      return false;
    }
  }

  if (positional < 2) {
    std::snprintf(err, errLen, "faltan las rutas de entrada y salida");
    return false;
  }
  if (opts.filterCount == 0) {
    for (int k = 0; k < FilterRegistry::mandatoryCount(); ++k) addUnique(opts, FilterRegistry::at(k));
  }
  opts.singleOutput = opts.filterCount == 1;
  return true;
}

void outputPathFor(const char* output, const char* filterName, char* buf, int bufLen) {
  const char* slash = std::strrchr(output, '/');
  const char* dot = std::strrchr(output, '.');
  if (dot == nullptr || dot == output || (slash != nullptr && dot < slash)) {
    std::snprintf(buf, bufLen, "%s_%s", output, filterName);
  } else {
    std::snprintf(buf, bufLen, "%.*s_%s%s", (int)(dot - output), output, filterName, dot);
  }
}

void printUsage(const char* prog) {
  std::fprintf(stderr, "uso: %s <entrada.pgm|ppm|-> <salida.pgm|ppm> [--f <filtro>]...\n", prog);
  std::fprintf(stderr, "  filtros:");
  for (int k = 0; k < FilterRegistry::count(); ++k) {
    std::fprintf(stderr, " %s", FilterRegistry::at(k)->name());
  }
  std::fprintf(stderr, " all\n");
  std::fprintf(stderr, "  sin --f se aplican blur, laplace y sharpen\n");
  std::fprintf(stderr, "  con varios filtros se escribe <salida>_<filtro>.<ext>\n");
}
