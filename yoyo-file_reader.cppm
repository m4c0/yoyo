module;
#include <stdio.h>

export module yoyo:file_reader;
import :reader;
import hai;
import missingno;

namespace yoyo {
export class file_reader : public reader {
  hai::c_file m_f;

  [[nodiscard]] static constexpr auto whence_of(seek_mode mode) noexcept {
    switch (mode) {
    case seek_mode::set:
      return SEEK_SET;
    case seek_mode::current:
      return SEEK_CUR;
    case seek_mode::end:
      return SEEK_END;
    }
  }

public:
  explicit file_reader(const char *name) : m_f{name, "rb"} {}

  [[nodiscard]] req<bool> eof() const noexcept override {
    return req<bool>{feof(*m_f) != 0};
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
} // namespace yoyo
