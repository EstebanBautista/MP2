#ifndef PNM_IO_H
#define PNM_IO_H

class Image;

class PnmReader {
 public:
  static bool read(const char* path, Image& out, char* err, int errLen);
  static bool parse(const char* buf, long len, Image& out, char* err, int errLen);
};

class PnmWriter {
 public:
  static long toBuffer(const Image& img, char*& buf);
  static bool write(const char* path, const Image& img, char* err, int errLen);
};

#endif
