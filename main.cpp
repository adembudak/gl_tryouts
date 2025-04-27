#include "Thing.h"

#include <memory>

int main() {
  auto an_app = std::make_unique<Thing>();
  an_app->run(std::move(an_app));
  return 0;
}
