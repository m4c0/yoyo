export module yoyo_libc:file_writer;
import :file_common;
import hai;
import missingno;
import yoyo;

namespace yoyo {
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
