module;
#include <stdio.h>

export module yoyo:libc;
import :common;
import :reader;
import :writer;
import hai;
import missingno;

namespace yoyo {
struct fcloser {
  void operator()(FILE *f) { fclose(f); }
};
using file = hai::holder<FILE, fcloser>;

constexpr const auto fclose = ::fclose;
constexpr const auto feof = ::feof;
constexpr const auto fopen = [](auto name, auto mode) {
#ifdef _WIN32
  FILE *res;
  return ::fopen_s(&res, name, mode) ? nullptr : res;
#else
  return ::fopen(name, mode);
#endif
};
constexpr const auto fprintf = ::fprintf;
constexpr const auto fread = ::fread;
constexpr const auto fseek = ::fseek;
constexpr const auto ftell = ::ftell;
constexpr const auto fwrite = ::fwrite;

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
  file m_f;

public:
  explicit file_reader(const char *name) : m_f{fopen(name, "rb")} {}

  [[nodiscard]] operator bool() const noexcept { return *m_f != nullptr; }

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

  [[nodiscard]] req<void> seekg(int pos, seek_mode mode) noexcept override {
    return fseek(*m_f, pos, whence_of(mode)) == 0
               ? req<void>{}
               : req<void>::failed("could not seek into file");
  }

  [[nodiscard]] req<unsigned> tellg() const noexcept override {
    unsigned res = ftell(*m_f);
    return res >= 0 ? req<unsigned>{res}
                    : req<unsigned>::failed("could not get file position");
  }
};

export class file_writer : public writer {
  file m_f;

public:
  explicit file_writer(const char *name) : m_f{fopen(name, "wb")} {}

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
