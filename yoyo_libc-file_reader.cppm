export module yoyo_libc:file_reader;
import :file_common;
import hai;
import missingno;
import yoyo;

namespace yoyo {
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
} // namespace yoyo
