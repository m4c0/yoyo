module;
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

export module yoyo:fd;
import :common;
import :reader;
import :writer;
import missingno;

namespace yoyo {
export class fd_reader : public reader {
  int m_fd;
  bool m_eof;

public:
  explicit constexpr fd_reader(int fd) noexcept : m_fd{fd} {}

  [[nodiscard]] bool ready() const noexcept override {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(m_fd, &set);

    timeval timeout{};

    return select(m_fd + 1, &set, NULL, NULL, &timeout) > 0;
  }

  [[nodiscard]] mno::req<bool> eof() const noexcept override {
    return mno::req{m_eof};
  }
  [[nodiscard]] mno::req<void> seekg(int pos,
                                     seek_mode mode) noexcept override {
    return mno::req<void>::failed("unsupported");
  }
  [[nodiscard]] mno::req<unsigned> tellg() const noexcept override {
    return mno::req<unsigned>::failed("unsupported");
  }
  [[nodiscard]] mno::req<void> read(void *buffer,
                                    unsigned len) noexcept override {
    return read_up_to(buffer, len)
        .assert([&](auto l) { return len == l; },
                "FD does not contain enough bytes")
        .map([](auto) {});
  }
  [[nodiscard]] mno::req<unsigned> read_up_to(void *buffer,
                                              unsigned len) noexcept override {
    auto rd = ::read(m_fd, buffer, len);
    if (rd == 0) {
      m_eof = true;
      return mno::req{0U};
    }

    if (rd != -1)
      return mno::req{static_cast<unsigned>(rd)};

    // let it leak until req can "own" the message
    return mno::req<unsigned>::failed(strdup(strerror(errno)));
  }
};

export class fd_writer : public writer {
  int m_fd;

public:
  explicit constexpr fd_writer(int fd) noexcept : m_fd{fd} {}

  [[nodiscard]] mno::req<void> seekp(int pos,
                                     seek_mode mode) noexcept override {
    return mno::req<void>::failed("unsupported");
  }
  [[nodiscard]] mno::req<unsigned> tellp() const noexcept override {
    return mno::req<unsigned>::failed("unsupported");
  }
  [[nodiscard]] mno::req<void> write(const void *buffer,
                                     unsigned len) noexcept override {
    if (-1 != ::write(m_fd, buffer, len))
      return {};

    // let it leak until req can "own" the message
    return mno::req<void>::failed(strdup(strerror(errno)));
  }
};
} // namespace yoyo
