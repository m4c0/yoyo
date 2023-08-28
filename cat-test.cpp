import yoyo;
import silog;

int main() {
  char buf[10240]{};
  yoyo::file_reader{"cat-test.cpp"}
      .read_up_to(buf, 6)
      .map([&](auto sz) {
        silog::log(silog::info, "Got %d bytes: [%s]", sz, buf);
      })
      .take([](auto err) { silog::log(silog::error, "Failed: %s", err); });
  yoyo::file_reader{"cat-test.cpp"}
      .read_up_to(buf, 10240)
      .map([&](auto sz) { silog::log(silog::info, "Got %d bytes", sz); })
      .take([](auto err) { silog::log(silog::error, "Failed: %s", err); });
}
