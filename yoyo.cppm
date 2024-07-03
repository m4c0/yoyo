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

constexpr auto write_u8(uint8_t n) {
  return [=](auto &w) { return w.write_u8(n); };
}
constexpr auto write_u16(uint16_t n) {
  return [=](auto &w) { return w.write_u16(n); };
}
constexpr auto write_u32_be(uint16_t n) {
  return [=](auto &w) { return w.write_u32_be(n); };
}
constexpr auto write(const uint8_t *data, unsigned size) {
  return [=](auto &w) { return w.write(data, size); };
}
constexpr auto write(const void *data, unsigned size) {
  return [=](auto &w) { return w.write(data, size); };
}
constexpr auto seek(int n, yoyo::seek_mode m) {
  return [=](auto &w) { return w.seekp(n, m); };
}
} // namespace yoyo
