#include "Thing.h"

int main() {
  Thing* an_app = new Thing;
  an_app->run(an_app);
  delete an_app;
  return 0;
}
