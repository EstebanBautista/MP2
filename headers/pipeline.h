#ifndef PIPELINE_H
#define PIPELINE_H

class Filter;
class Image;

typedef void (*FilterStrategy)(const Filter& f, const Image& src, Image& dst);

int runPipeline(const char* design, int threads, int argc, char* argv[], FilterStrategy strategy);

#endif
