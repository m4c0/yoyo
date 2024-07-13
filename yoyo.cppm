export module yoyo;
export import missingno;
export import :ce_reader;
export import :common;
export import :memreader;
export import :memwriter;
export import :reader;
export import :subreader;
export import :writer;
import traits;

#ifdef __APPLE__
export import :fd;
#endif

#ifndef __wasm__
export import :libc;
#endif

using namespace traits::ints;

export namespace yoyo {
constexpr auto read(void *data, unsigned size) {
  return [=](auto &r) { return r.read(data, size); };
}
constexpr auto read_u16(uint16_t &n) {
  return [&](auto &r) { return r.read_u16().map([&](auto v) { n = v; }); };
}
constexpr auto read_u32(uint32_t &n) {
  return [&](auto &r) { return r.read_u32().map([&](auto v) { n = v; }); };
}

constexpr auto until_eof(auto &&fn) {
  return [&](auto &r) {
    mno::req<void> res{};
    while (res.is_valid() && !r.eof().unwrap(false)) {
      res = fn(r);
    }
    return res;
  };
};

constexpr auto write_u8(uint8_t n) {
  return [=](auto &w) { return w.write_u8(n); };
}
constexpr auto write_u16(uint16_t n) {
  return [=](auto &w) { return w.write_u16(n); };
}
constexpr auto write_u32(uint32_t n) {
  return [=](auto &w) { return w.write_u32(n); };
}
constexpr auto write_u64(uint64_t n) {
  return [=](auto &w) { return w.write_u64(n); };
}
constexpr auto write_u8_be(uint8_t n) {
  return [=](auto &w) { return w.write_u8_be(n); };
}
constexpr auto write_u16_be(uint16_t n) {
  return [=](auto &w) { return w.write_u16_be(n); };
}
constexpr auto write_u32_be(uint32_t n) {
  return [=](auto &w) { return w.write_u32_be(n); };
}
constexpr auto write_u64_be(uint64_t n) {
  return [=](auto &w) { return w.write_u64_be(n); };
}
constexpr auto write(const uint8_t *data, unsigned size) {
  return [=](auto &w) { return w.write(data, size); };
}
constexpr auto write(const void *data, unsigned size) {
  return [=](auto &w) { return w.write(data, size); };
}

constexpr auto seekg(int n, yoyo::seek_mode m) {
  return [=](auto &w) { return w.seekg(n, m); };
}
constexpr auto seekp(int n, yoyo::seek_mode m) {
  return [=](auto &w) { return w.seekp(n, m); };
}
} // namespace yoyo
