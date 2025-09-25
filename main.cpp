#include "App.h"

#include <memory>

int main() {
  auto an_app = std::make_unique<App>();
  an_app->run(std::move(an_app));
  return 0;
}
