module;
#include <stdio.h>

export module yoyo_libc:file_writer;
import hai;
import missingno;
import yoyo;

namespace yoyo {
struct fcloser {
  void operator()(FILE *f) { fclose(f); }
};
export class file_writer : public writer {
  hai::holder<FILE, fcloser> m_f;

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
  explicit file_writer(const char *name) : m_f{fopen(name, "wb")} {}

  [[nodiscard]] mno::req<void> write(const void *buffer,
                                     unsigned len) noexcept override {
    return fwrite(buffer, len, 1, *m_f) == 1
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
