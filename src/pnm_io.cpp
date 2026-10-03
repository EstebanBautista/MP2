#include "pnm_io.h"

#include <cstdio>
#include <cstring>
#include <utility>

#include "image.h"

namespace {

const long kMaxDimension = 100000;
const long kMaxPixels = 200000000L;

void setError(char* err, int errLen, const char* message) {
  if (err != nullptr && errLen > 0) std::snprintf(err, errLen, "%s", message);
}

bool isSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f';
}

bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }

struct Cursor {
  const char* p;
  const char* end;
};

void skipSpaceAndComments(Cursor& c) {
  while (c.p < c.end) {
    if (*c.p == '#') {
      while (c.p < c.end && *c.p != '\n') ++c.p;
    } else if (isSpace(*c.p)) {
      ++c.p;
    } else {
      break;
    }
  }
}

bool readInt(Cursor& c, long& value) {
  skipSpaceAndComments(c);
  if (c.p >= c.end || !isDigit(*c.p)) return false;
  long v = 0;
  while (c.p < c.end && isDigit(*c.p)) {
    v = v * 10 + (*c.p - '0');
    if (v > 1000000000L) return false;
    ++c.p;
  }
  value = v;
  return true;
}

char* readAll(FILE* file, long& len) {
  long capacity = 1L << 20;
  char* buf = new char[capacity];
  len = 0;
  for (;;) {
    if (len == capacity) {
      long bigger = capacity * 2;
      char* grown = new char[bigger];
      std::memcpy(grown, buf, (size_t)len);
      delete[] buf;
      buf = grown;
      capacity = bigger;
    }
    size_t n = std::fread(buf + len, 1, (size_t)(capacity - len), file);
    if (n == 0) break;
    len += (long)n;
  }
  return buf;
}

char* writeInt(char* p, int value) {
  char digits[12];
  int n = 0;
  if (value == 0) digits[n++] = '0';
  while (value > 0) {
    digits[n++] = (char)('0' + value % 10);
    value /= 10;
  }
  while (n > 0) *p++ = digits[--n];
  return p;
}

}  // namespace

bool PnmReader::parse(const char* buf, long len, Image& out, char* err, int errLen) {
  Cursor c = {buf, buf + len};
  skipSpaceAndComments(c);
  if (c.end - c.p < 2 || c.p[0] != 'P' || (c.p[1] != '2' && c.p[1] != '3')) {
    setError(err, errLen, "numero magico invalido (se espera P2 o P3)");
    return false;
  }
  const int channels = c.p[1] == '3' ? 3 : 1;
  c.p += 2;

  long width = 0, height = 0, maxval = 0;
  if (!readInt(c, width) || !readInt(c, height) || !readInt(c, maxval)) {
    setError(err, errLen, "encabezado incompleto (ancho, alto y maxval)");
    return false;
  }
  if (width <= 0 || height <= 0 || width > kMaxDimension || height > kMaxDimension ||
      width * height > kMaxPixels) {
    setError(err, errLen, "dimensiones invalidas");
    return false;
  }
  if (maxval <= 0 || maxval > 65535) {
    setError(err, errLen, "maxval invalido (debe estar entre 1 y 65535)");
    return false;
  }

  Image img((int)width, (int)height, channels, (int)maxval);
  const long n = img.size();
  int* data = img.data();
  for (long i = 0; i < n; ++i) {
    long value = 0;
    if (!readInt(c, value)) {
      setError(err, errLen, "faltan valores de pixel");
      return false;
    }
    if (value > maxval) {
      setError(err, errLen, "valor de pixel mayor que maxval");
      return false;
    }
    data[i] = (int)value;
  }
  out = std::move(img);
  return true;
}

bool PnmReader::read(const char* path, Image& out, char* err, int errLen) {
  const bool useStdin = std::strcmp(path, "-") == 0;
  FILE* file = useStdin ? stdin : std::fopen(path, "rb");
  if (file == nullptr) {
    if (err != nullptr && errLen > 0) std::snprintf(err, errLen, "no se pudo abrir '%s'", path);
    return false;
  }
  long len = 0;
  char* buf = readAll(file, len);
  if (!useStdin) std::fclose(file);
  const bool ok = parse(buf, len, out, err, errLen);
  delete[] buf;
  return ok;
}

long PnmWriter::toBuffer(const Image& img, char*& buf) {
  const long n = img.size();
  buf = new char[64 + n * 7];
  int headerLen = std::snprintf(buf, 64, "%s\n%d %d\n%d\n", img.magic(), img.width(),
                                img.height(), img.maxval());
  char* p = buf + headerLen;
  const int* data = img.data();
  const long rowLen = (long)img.width() * img.channels();
  for (long i = 0; i < n; ++i) {
    p = writeInt(p, data[i]);
    *p++ = ((i + 1) % rowLen == 0) ? '\n' : ' ';
  }
  return (long)(p - buf);
}

bool PnmWriter::write(const char* path, const Image& img, char* err, int errLen) {
  FILE* file = std::fopen(path, "wb");
  if (file == nullptr) {
    if (err != nullptr && errLen > 0) std::snprintf(err, errLen, "no se pudo escribir '%s'", path);
    return false;
  }
  char* buf = nullptr;
  const long len = toBuffer(img, buf);
  const size_t written = std::fwrite(buf, 1, (size_t)len, file);
  delete[] buf;
  const bool closed = std::fclose(file) == 0;
  if (written != (size_t)len || !closed) {
    if (err != nullptr && errLen > 0) std::snprintf(err, errLen, "no se pudo escribir '%s'", path);
    return false;
  }
  return true;
}
