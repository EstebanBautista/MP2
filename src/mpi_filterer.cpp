#include <mpi.h>

#include <cstdio>
#include <cstring>

#include "cli.h"
#include "convolver.h"
#include "filter.h"
#include "image.h"
#include "pnm_io.h"
#include "strips.h"
#include "timer.h"

namespace {

const int kMaxRanks = 64;
const int kTagStrip = 1;
const int kStats = 6;  // comm, filterWall, filterCpu, totalWall, totalCpu, write

char g_hosts[kMaxRanks][MPI_MAX_PROCESSOR_NAME];
double g_stats[kMaxRanks * kStats];

}  // namespace

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  int rank = 0, size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size > kMaxRanks) {
    if (rank == 0) std::fprintf(stderr, "error: maximo %d procesos\n", kMaxRanks);
    MPI_Finalize();
    return 1;
  }

  CliOptions opts;
  char err[256];
  if (!parseCli(argc, argv, opts, err, sizeof err)) {
    if (rank == 0) {
      std::fprintf(stderr, "error: %s\n", err);
      printUsage(argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  char host[MPI_MAX_PROCESSOR_NAME];
  std::memset(host, 0, sizeof host);
  int hostLen = 0;
  MPI_Get_processor_name(host, &hostLen);
  MPI_Gather(host, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, g_hosts, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0,
             MPI_COMM_WORLD);
  if (rank == 0) {
    for (int r = 0; r < size; ++r) std::printf("NODE rank=%d host=%s\n", r, g_hosts[r]);
  }

  Timer t;
  Image full;
  int header[4] = {0, 0, 0, 0};
  int ok = 1;
  double readMs = 0, readCpu = 0;
  if (rank == 0) {
    if (PnmReader::read(opts.input, full, err, sizeof err)) {
      header[0] = full.width();
      header[1] = full.height();
      header[2] = full.channels();
      header[3] = full.maxval();
    } else {
      std::fprintf(stderr, "error: %s\n", err);
      ok = 0;
    }
    readMs = t.wallMs();
    readCpu = t.cpuMs();
  }
  MPI_Bcast(&ok, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (!ok) {
    MPI_Finalize();
    return 2;
  }
  MPI_Bcast(header, 4, MPI_INT, 0, MPI_COMM_WORLD);
  const int width = header[0], height = header[1], channels = header[2], maxval = header[3];
  const int rowInts = width * channels;

  int y0[kMaxRanks], y1[kMaxRanks];
  computeStrips(height, size, y0, y1);
  int h0, h1;
  haloBounds(height, y0[rank], y1[rank], h0, h1);

  int counts[kMaxRanks], displs[kMaxRanks];
  for (int r = 0; r < size; ++r) {
    counts[r] = (y1[r] - y0[r]) * rowInts;
    displs[r] = y0[r] * rowInts;
  }

  t.start();
  Image local(width, h1 - h0, channels, maxval);
  if (rank == 0) {
    for (int r = 1; r < size; ++r) {
      int rh0, rh1;
      haloBounds(height, y0[r], y1[r], rh0, rh1);
      const int count = (rh1 - rh0) * rowInts;
      if (count > 0) {
        MPI_Send(full.data() + (long)rh0 * rowInts, count, MPI_INT, r, kTagStrip, MPI_COMM_WORLD);
      }
    }
    if (h1 > h0) {
      std::memcpy(local.data(), full.data() + (long)h0 * rowInts,
                  sizeof(int) * (size_t)(h1 - h0) * rowInts);
    }
  } else if (h1 > h0) {
    MPI_Recv(local.data(), (h1 - h0) * rowInts, MPI_INT, 0, kTagStrip, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
  }
  const double scatterMs = t.wallMs();
  const double scatterCpu = t.cpuMs();

  for (int i = 0; i < opts.filterCount; ++i) {
    const Filter& f = *opts.filters[i];
    Image localDst = local.cloneEmpty();

    t.start();
    Convolver::applyRegion(f, local, localDst, 0, width, y0[rank] - h0, y1[rank] - h0);
    const double filterWall = t.wallMs();
    const double filterCpu = t.cpuMs();

    Image result;
    if (rank == 0) result = full.cloneEmpty();
    t.start();
    const int* sendPtr = localDst.data() + (long)(y0[rank] - h0) * rowInts;
    MPI_Gatherv(sendPtr, counts[rank], MPI_INT, rank == 0 ? result.data() : nullptr, counts,
                displs, MPI_INT, 0, MPI_COMM_WORLD);
    const double gatherMs = t.wallMs();
    const double gatherCpu = t.cpuMs();

    double writeMs = 0, writeCpu = 0;
    int writeOk = 1;
    if (rank == 0) {
      char path[1024];
      if (opts.singleOutput) {
        std::snprintf(path, sizeof path, "%s", opts.output);
      } else {
        outputPathFor(opts.output, f.name(), path, sizeof path);
      }
      t.start();
      if (!PnmWriter::write(path, result, err, sizeof err)) {
        std::fprintf(stderr, "error: %s\n", err);
        writeOk = 0;
      }
      writeMs = t.wallMs();
      writeCpu = t.cpuMs();
    }

    double mine[kStats] = {scatterMs + gatherMs,
                           filterWall,
                           filterCpu,
                           readMs + scatterMs + filterWall + gatherMs + writeMs,
                           readCpu + scatterCpu + filterCpu + gatherCpu + writeCpu,
                           writeMs};
    MPI_Gather(mine, kStats, MPI_DOUBLE, g_stats, kStats, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
      for (int r = 0; r < size; ++r) {
        const double* s = g_stats + r * kStats;
        TimeReport rep = {};
        rep.design = "mpi";
        rep.image = baseName(opts.input);
        rep.filter = f.name();
        rep.threads = 1;
        rep.nodes = size;
        rep.rank = r;
        rep.readMs = r == 0 ? readMs : 0;
        rep.commMs = s[0];
        rep.filterWallMs = s[1];
        rep.filterCpuMs = s[2];
        rep.totalWallMs = s[3];
        rep.totalCpuMs = s[4];
        rep.writeMs = s[5];
        printTime(rep);
      }
    }

    MPI_Bcast(&writeOk, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (!writeOk) {
      MPI_Finalize();
      return 3;
    }
  }

  MPI_Finalize();
  return 0;
}
