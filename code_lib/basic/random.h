#ifndef RANDOM_H
#define RANDOM_H

#include "maths.h"

#include <stdlib.h>
#include <time.h>

internal s32
get_random() {
  static bool srand_init = false;
  if(!srand_init) {
    srand_init = true;
    srand((u32) time(0));
  }
  return rand();
}

#endif // !RANDOM_H
