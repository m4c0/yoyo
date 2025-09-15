module;
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

export module yoyo:fd;
import :common;
import :reader;
import :writer;
import jute;
import missingno;
import traits;

using i64 = traits::ints::int64_t;
using u64 = traits::ints::uint64_t;

namespace yoyo {
export class fd_reader : public reader {
  int m_fd;
  bool m_eof;

public:
  explicit constexpr fd_reader(int fd) : m_fd{fd} {}

  [[nodiscard]] bool ready() const override {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_fd, &set);

    timeval timeout{};

    return select(m_fd + 1, &set, NULL, NULL, &timeout) > 0;
  }

  [[nodiscard]] mno::req<bool> eof() const override { return mno::req{m_eof}; }
  [[nodiscard]] mno::req<void> seekg(i64 pos, seek_mode mode) override {
    return mno::req<void>::failed("unsupported");
  }
  [[nodiscard]] mno::req<u64> tellg() const override {
    return mno::req<u64>::failed("unsupported");
  }
  [[nodiscard]] mno::req<void> read(void *buffer, unsigned len) override {
    return read_up_to(buffer, len)
        .assert([&](auto l) { return len == l; },
                "FD does not contain enough bytes")
        .map([](auto) {});
  }
  [[nodiscard]] mno::req<unsigned> read_up_to(void *buffer,
                                              unsigned len) override {
    auto rd = ::read(m_fd, buffer, len);
    if (rd == 0) {
      m_eof = true;
      return mno::req{0U};
    }

    if (rd != -1)
      return mno::req{static_cast<unsigned>(rd)};

    return mno::req<unsigned>::failed(jute::heap{jute::view::unsafe(strerror(errno))});
  }
};

export class fd_writer : public writer {
  int m_fd;

public:
  explicit constexpr fd_writer(int fd) : m_fd{fd} {}

  [[nodiscard]] mno::req<void> seekp(int pos, seek_mode mode) override {
    return mno::req<void>::failed("unsupported");
  }
  [[nodiscard]] mno::req<unsigned> tellp() const override {
    return mno::req<unsigned>::failed("unsupported");
  }
  [[nodiscard]] mno::req<void> write(const void *buffer,
                                     unsigned len) override {
    if (-1 != ::write(m_fd, buffer, len))
      return {};

    return mno::req<void>::failed(jute::heap{jute::view::unsafe(strerror(errno))});
  }
};
} // namespace yoyo
