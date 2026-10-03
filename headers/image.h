#ifndef IMAGE_H
#define IMAGE_H

class Image {
 public:
  Image();
  Image(int width, int height, int channels, int maxval);
  ~Image();

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
  Image(Image&& other) noexcept;
  Image& operator=(Image&& other) noexcept;

  int width() const { return width_; }
  int height() const { return height_; }
  int channels() const { return channels_; }
  int maxval() const { return maxval_; }
  const char* magic() const { return channels_ == 3 ? "P3" : "P2"; }
  long size() const { return (long)width_ * height_ * channels_; }
  bool empty() const { return data_ == nullptr; }

  int* data() { return data_; }
  const int* data() const { return data_; }

  int at(int x, int y, int c) const {
    return data_[((long)y * width_ + x) * channels_ + c];
  }
  void set(int x, int y, int c, int value) {
    data_[((long)y * width_ + x) * channels_ + c] = value;
  }

  Image cloneEmpty() const;

 private:
  void release();
  void takeFrom(Image& other);

  int width_;
  int height_;
  int channels_;
  int maxval_;
  int* data_;
};

#endif
