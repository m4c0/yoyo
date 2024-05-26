#pragma leco tool

import yoyo;
import silog;

int main() {
  char buf[10240]{};
  yoyo::file_reader::open("cat-test.cpp")
      .fmap([&](auto &&f) { return f.read_up_to(buf, 6); })
      .map([&](auto sz) {
        silog::log(silog::info, "Got %d bytes: [%s]", sz, buf);
      })
      .log_error();
  yoyo::file_reader::open("cat-test.cpp")
      .fmap([&](auto &&f) { return f.read_up_to(buf, 10240); })
      .map([&](auto sz) { silog::log(silog::info, "Got %d bytes", sz); })
      .log_error();
}
