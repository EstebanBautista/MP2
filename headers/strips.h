#ifndef STRIPS_H
#define STRIPS_H

#include "image.h"

void computeStrips(int height, int parts, int* y0, int* y1);
void haloBounds(int height, int y0, int y1, int& h0, int& h1);
Image copyRows(const Image& src, int y0, int y1);

#endif
