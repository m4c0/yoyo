#pragma leco tool

import hai;
import jute;
import yoyo;

struct header {
  char name[100];
  char mode[8];
  char owner[8];
  char group[8];
  char size[12];
  char modtime[12];
  char checksum[8];
  char type = '0';
  char linked[100];
  char magic[6] = "ustar";
  char version[2]{'0', '0'};
  char owner_name[32] = "ecosta";
  char group_name[32] = "staff";
  char dev_major[8];
  char dev_minor[8];
  char prefix[155];
  char pad[512 - 345 - 155];
};
static_assert(sizeof(header) == 512);

template <unsigned N>
inline void set_octal(char (&cs)[N], unsigned val, unsigned len = N - 1) {
  for (auto i = 0; i < len; i++) {
    cs[len - i - 1] = (val % 8) + '0';
    val /= 8;
  }
  cs[len] = ' ';
}

void failure(jute::view) { throw 0; }

inline void add_file(const char *name, yoyo::writer *out) {
  auto in = yoyo::file_reader::open(name).take(failure);
  auto size = in.size().take(failure);
  auto buf = hai::holder<char[]>::make(size);
  in.read(*buf, size).take(failure);

  header h{};
  set_octal(h.mode, 0644, 6);
  set_octal(h.owner, 0766, 6);
  set_octal(h.group, 024, 6);
  set_octal(h.size, size);
  set_octal(h.modtime, 014363555341);
  set_octal(h.dev_major, 0, 6);
  set_octal(h.dev_minor, 0, 6);

  for (auto i = 0; i < 8; i++)
    h.checksum[i] = ' ';
  for (auto i = 0; name[i]; i++)
    h.name[i] = name[i];

  auto *hdata = reinterpret_cast<unsigned char *>(&h);
  unsigned checksum = 0;
  for (auto i = 0; i < 512; i++) {
    checksum += hdata[i];
  }
  set_octal(h.checksum, checksum, 6);
  h.checksum[6] = 0;

  out->write(h).take(failure);
  out->write(*buf, size).take(failure);

  char pad[512]{};
  unsigned pad_sz = 512 - (size % 512);
  if (pad_sz != 512)
    out->write(pad, pad_sz).take(failure);
}

int main() {
  auto out = yoyo::file_writer::open("out/tar-test.tar").take(failure);
  add_file("build.cpp", &out);
  add_file("subreader-test.cpp", &out);

  char pad[0x1400]{};
  out.write(pad, sizeof(pad)).take(failure);
}
