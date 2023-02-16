module;
#include <stdio.h>

export module yoyo_libc:file_common;
import hai;
import yoyo;

namespace yoyo {
struct fcloser {
  void operator()(FILE *f) { fclose(f); }
};
using file = hai::holder<FILE, fcloser>;

constexpr const auto fclose = ::fclose;
constexpr const auto feof = ::feof;
constexpr const auto fopen = ::fopen;
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

} // namespace yoyo
