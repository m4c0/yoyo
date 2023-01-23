#include "build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto all = unit::create<seq>("all");

  auto tst = all->add_unit<seq>("tests");
  tst->add_ref(yoyo());
  tst->add_unit<>("subreader-test");

  all->add_unit<per_feat<seq>>("pf")->for_feature(posix).add_ref(yoyo_libc());

  return run_main(all, argc, argv);
}
