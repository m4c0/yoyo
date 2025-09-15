module;
#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef LECO_TARGET_WINDOWS
#define ftell64 _ftelli64
#define fseek64 _fseeki64
#else
#define ftell64 ftello
#define fseek64 fseeko
#endif

export module yoyo:libc;
import :common;
import :reader;
import :writer;
import hai;
import jute;
import missingno;
import traits;

using i64 = traits::ints::int64_t;
using u64 = traits::ints::uint64_t;

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
#define strerror_r(err, buf, len) strerror_s(buf, len, err)
#endif

[[nodiscard]] inline constexpr auto whence_of(seek_mode mode) {
  switch (mode) {
  case seek_mode::set:
    return SEEK_SET;
  case seek_mode::current:
    return SEEK_CUR;
  case seek_mode::end:
    return SEEK_END;
  }
}

[[nodiscard]] inline jute::heap perror(jute::view msg) {
  char buf[1024]{};
  if (strerror_r(errno, buf, sizeof(buf)) != 0) {
    return jute::heap { msg };
  }
  return msg + ": " + jute::view::unsafe(buf);
}

export class file_reader : public reader {
  file m_f{};

public:
  constexpr file_reader() = default;
  explicit constexpr file_reader(FILE *f) : m_f{f} {}

  [[nodiscard]] static req<file_reader> open(const char *name) {
    using namespace jute::literals;

    auto f = fopen(name, "rb");
    return f == nullptr ? req<file_reader>::failed("failed to open file: "_s +
                                                   jute::view::unsafe(name))
                        : req<file_reader>{file_reader{f}};
  }
  [[nodiscard]] static req<file_reader> std_in() {
    return req<file_reader>{file_reader{stdin}};
  }

  [[nodiscard]] req<bool> eof() const override {
    return req<bool>{feof(*m_f) != 0};
  }

  [[nodiscard]] req<unsigned> read_up_to(void *buffer, unsigned len) override {
    unsigned l = fread(buffer, 1, len, *m_f);
    return l >= 0 ? req<unsigned>{l}
                  : req<unsigned>::failed(perror("could not read file"));
  }
  [[nodiscard]] req<void> read(void *buffer, unsigned len) override {
    if (fread(buffer, len, 1, *m_f) == 1)
      return req<void>{};

    if (feof(*m_f))
      return req<void>::failed("EOF while reading file");

    return req<void>::failed(perror("could not read a block from file"));
  }
  using reader::read;

  [[nodiscard]] req<void> readline(char *buffer, unsigned len) {
    if (fgets(buffer, len, *m_f))
      return req<void>{};

    if (feof(*m_f))
      return req<void>::failed("EOF while reading line from file");

    return req<void>::failed(perror("could not read a line from file"));
  }

  [[nodiscard]] req<void> seekg(i64 pos, seek_mode mode) override {
    return fseek64(*m_f, pos, whence_of(mode)) == 0
               ? req<void>{}
               : req<void>::failed("could not seek into file");
  }

  [[nodiscard]] req<u64> tellg() const override {
    // This might be a problem on platforms with sizeof(long) == 4 dealing with
    // files larger than ~2GB.
    i64 res = ftell64(*m_f);
    return res >= 0 ? req<u64>{static_cast<u64>(res)}
                    : req<u64>::failed("could not get file position");
  }
};

export class file_writer : public writer {
  file m_f{};

public:
  constexpr file_writer() = default;
  explicit constexpr file_writer(FILE *f) : m_f{f} {}

  [[nodiscard]] static mno::req<file_writer> open(const char *name) {
    using namespace jute::literals;

    auto f = fopen(name, "wb");
    return f == nullptr
               ? mno::req<file_writer>::failed("failed to open file: "_s +
                                               jute::view::unsafe(name))
               : mno::req<file_writer>{file_writer{f}};
  }

  [[nodiscard]] static mno::req<file_writer> append(const char *name) {
    auto f = fopen(name, "ab");
    return f == nullptr ? mno::req<file_writer>::failed(
                              "failed to open file for appending")
                        : mno::req<file_writer>{file_writer{f}};
  }

  using writer::write;
  [[nodiscard]] mno::req<void> write(const void *buffer,
                                     unsigned len) override {
    return fwrite(buffer, len, 1, *m_f) == 1
               ? mno::req<void>{}
               : mno::req<void>::failed("could not write file");
  }

  [[nodiscard]] mno::req<void> writef(const char *fmt, auto... args) {
    return fprintf(*m_f, fmt, args...) > 0
               ? mno::req<void>{}
               : mno::req<void>::failed("could not write file");
  }

  [[nodiscard]] mno::req<void> seekp(int pos, seek_mode mode) override {
    return fseek(*m_f, pos, whence_of(mode)) == 0
               ? mno::req<void>{}
               : mno::req<void>::failed("could not seek into file");
  }

  [[nodiscard]] mno::req<unsigned> tellp() const override {
    unsigned res = ftell(*m_f);
    return res >= 0 ? mno::req<unsigned>{res}
                    : mno::req<unsigned>::failed("could not get file position");
  }
};
} // namespace yoyo
