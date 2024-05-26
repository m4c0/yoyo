module;
#define _FILE_OFFSET_BITS 64
#include <stdio.h>

#ifdef LECO_TARGET_WINDOWS
#define ftell64 _ftelli64
#define fseek64 _fseeki64
#else
#define ftell64 ftello64
#define fseek64 fseeko64
#endif

export module yoyo:libc;
import :common;
import :reader;
import :writer;
import hai;
import missingno;
import traits;

using namespace traits::ints;

namespace yoyo {
struct fcloser {
  void operator()(FILE *f) { fclose(f); }
};
using file = hai::holder<FILE, fcloser>;

#ifdef _WIN32
static inline FILE *fopen(auto name, auto mode) {
  FILE *res;
  return ::fopen_s(&res, name, mode) ? nullptr : res;
}
#endif

[[nodiscard]] inline constexpr auto whence_of(seek_mode mode) noexcept {
  switch (mode) {
  case seek_mode::set:
    return SEEK_SET;
  case seek_mode::current:
    return SEEK_CUR;
  case seek_mode::end:
    return SEEK_END;
  }
}

export class file_reader : public reader {
  file m_f{};

public:
  constexpr file_reader() = default;
  explicit constexpr file_reader(FILE *f) : m_f{f} {}

  [[nodiscard]] static req<file_reader> open(const char *name) {
    auto f = fopen(name, "rb");
    return f == nullptr ? req<file_reader>::failed("failed to open file")
                        : req<file_reader>{file_reader{f}};
  }

  [[nodiscard]] req<bool> eof() const noexcept override {
    return req<bool>{feof(*m_f) != 0};
  }

  [[nodiscard]] req<unsigned> read_up_to(void *buffer,
                                         unsigned len) noexcept override {
    unsigned l = fread(buffer, 1, len, *m_f);
    return l >= 0 ? req<unsigned>{l}
                  : req<unsigned>::failed("could not read file");
  }
  [[nodiscard]] req<void> read(void *buffer, unsigned len) noexcept override {
    return fread(buffer, len, 1, *m_f) == 1
               ? req<void>{}
               : req<void>::failed("could not read file");
  }

  [[nodiscard]] req<void> seekg(int64_t pos, seek_mode mode) noexcept override {
    return fseek64(*m_f, pos, whence_of(mode)) == 0
               ? req<void>{}
               : req<void>::failed("could not seek into file");
  }

  [[nodiscard]] req<uint64_t> tellg() const noexcept override {
    // This might be a problem on platforms with sizeof(long) == 4 dealing with
    // files larger than ~2GB.
    int64_t res = ftell64(*m_f);
    return res >= 0 ? req<uint64_t>{static_cast<uint64_t>(res)}
                    : req<uint64_t>::failed("could not get file position");
  }
};

export class file_writer : public writer {
  file m_f{};

public:
  constexpr file_writer() = default;
  explicit constexpr file_writer(FILE *f) : m_f{f} {}

  [[nodiscard]] static mno::req<file_writer> open(const char *name) {
    auto f = fopen(name, "wb");
    return f == nullptr ? mno::req<file_writer>::failed("failed to open file")
                        : mno::req<file_writer>{file_writer{f}};
  }

  using writer::write;
  [[nodiscard]] mno::req<void> write(const void *buffer,
                                     unsigned len) noexcept override {
    return fwrite(buffer, len, 1, *m_f) == 1
               ? mno::req<void>{}
               : mno::req<void>::failed("could not write file");
  }

  [[nodiscard]] mno::req<void> writef(const char *fmt, auto... args) noexcept {
    return fprintf(*m_f, fmt, args...) > 0
               ? mno::req<void>{}
               : mno::req<void>::failed("could not write file");
  }

  [[nodiscard]] mno::req<void> seekp(int pos,
                                     seek_mode mode) noexcept override {
    return fseek(*m_f, pos, whence_of(mode)) == 0
               ? mno::req<void>{}
               : mno::req<void>::failed("could not seek into file");
  }

  [[nodiscard]] mno::req<unsigned> tellp() const noexcept override {
    unsigned res = ftell(*m_f);
    return res >= 0 ? mno::req<unsigned>{res}
                    : mno::req<unsigned>::failed("could not get file position");
  }
};
} // namespace yoyo
