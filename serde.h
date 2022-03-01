#ifndef TCEL_SERDE_H_
#define TCEL_SERDE_H_

#include <stdbool.h>

#include "common.h"

enum err process(char *filename, enum err (*handler)(struct sect *rec, void *userdata), void *userdata, bool dump);

#endif
