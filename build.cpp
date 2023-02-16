#include "build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto all = unit::create<seq>("all");

  auto tst = all->add_unit<seq>("tests");
  tst->add_ref(yoyo());
  tst->add_unit<>("subreader-test");

  auto tar = all->add_unit<tool>("tar-test");
  tar->add_wsdep("hai", hai());
  tar->add_ref(yoyo_libc());
  tar->add_unit("tar-test");

  return run_main(all, argc, argv);
}
