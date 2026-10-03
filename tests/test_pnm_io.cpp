#include <cstring>

#include "image.h"
#include "pnm_io.h"
#include "test.h"

static bool parseText(const char* text, Image& out, char* err) {
  return PnmReader::parse(text, (long)std::strlen(text), out, err, 256);
}

void test_pnm_io() {
  char err[256];

  {
    Image img;
    CHECK(parseText("P2\n3 2\n255\n255 0 0\n0 255 0\n", img, err));
    CHECK(img.channels() == 1 && img.width() == 3 && img.height() == 2);
    CHECK(img.maxval() == 255);
    CHECK(img.at(0, 0, 0) == 255 && img.at(1, 1, 0) == 255 && img.at(2, 1, 0) == 0);
  }
  {
    Image img;
    CHECK(parseText("P2\n# creado a mano\n2 1 # ancho alto\n# max\n15\n1 # px\n15\n", img, err));
    CHECK(img.width() == 2 && img.maxval() == 15 && img.at(1, 0, 0) == 15);
  }
  {
    Image img;
    CHECK(parseText("P3\n2 1\n255\n255 0 0  0 255 0\n", img, err));
    CHECK(img.channels() == 3 && img.at(0, 0, 0) == 255 && img.at(1, 0, 1) == 255);
  }
  {
    Image img;  // crlf
    CHECK(parseText("P2\r\n2 1\r\n255\r\n7 8\r\n", img, err));
    CHECK(img.at(0, 0, 0) == 7 && img.at(1, 0, 0) == 8);
  }
  {
    Image img;
    CHECK(!parseText("P5\n1 1\n255\n0\n", img, err));
    CHECK(std::strstr(err, "magico") != nullptr);
  }
  {
    Image img;
    CHECK(!parseText("P2\n2\n", img, err));
    CHECK(std::strstr(err, "encabezado") != nullptr);
  }
  {
    Image img;
    CHECK(!parseText("P2\n0 1\n255\n", img, err));
    CHECK(std::strstr(err, "dimensiones") != nullptr);
  }
  {
    Image img;
    CHECK(!parseText("P2\n1 1\n70000\n0\n", img, err));
    CHECK(std::strstr(err, "maxval") != nullptr);
  }
  {
    Image img;
    CHECK(!parseText("P2\n2 2\n255\n1 2 3\n", img, err));
    CHECK(std::strstr(err, "faltan valores") != nullptr);
  }
  {
    Image img;
    CHECK(!parseText("P2\n1 1\n15\n16\n", img, err));
    CHECK(std::strstr(err, "mayor que maxval") != nullptr);
  }
  {
    Image src(3, 2, 3, 255);
    fillPattern(src);
    char* buf = nullptr;
    long len = PnmWriter::toBuffer(src, buf);
    Image back;
    CHECK(PnmReader::parse(buf, len, back, err, 256));
    delete[] buf;
    CHECK(sameImage(src, back));
  }
  {
    Image src(5, 4, 1, 15);
    fillPattern(src);
    CHECK(PnmWriter::write("output/test_roundtrip.pgm", src, err, 256));
    Image back;
    CHECK(PnmReader::read("output/test_roundtrip.pgm", back, err, 256));
    CHECK(sameImage(src, back));
  }
  {
    Image img;
    CHECK(!PnmReader::read("output/no_existe.pgm", img, err, 256));
    CHECK(std::strstr(err, "no se pudo abrir") != nullptr);
  }
  {
    Image src(1, 1, 1, 255);
    CHECK(!PnmWriter::write("output/carpeta_inexistente/x.pgm", src, err, 256));
    CHECK(std::strstr(err, "no se pudo escribir") != nullptr);
  }
}
