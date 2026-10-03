#include "image.h"

Image::Image() : width_(0), height_(0), channels_(0), maxval_(0), data_(nullptr) {}

Image::Image(int width, int height, int channels, int maxval)
    : width_(width), height_(height), channels_(channels), maxval_(maxval),
      data_(new int[(long)width * height * channels]()) {}

Image::~Image() { release(); }

Image::Image(Image&& other) noexcept : data_(nullptr) { takeFrom(other); }

Image& Image::operator=(Image&& other) noexcept {
  if (this != &other) {
    release();
    takeFrom(other);
  }
  return *this;
}

Image Image::cloneEmpty() const { return Image(width_, height_, channels_, maxval_); }

void Image::release() {
  delete[] data_;
  data_ = nullptr;
}

void Image::takeFrom(Image& other) {
  width_ = other.width_;
  height_ = other.height_;
  channels_ = other.channels_;
  maxval_ = other.maxval_;
  data_ = other.data_;
  other.width_ = other.height_ = other.channels_ = other.maxval_ = 0;
  other.data_ = nullptr;
}
