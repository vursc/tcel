#ifndef TCEL_SERDE_H_
#define TCEL_SERDE_H_

// process(), declared here and defined in serde.c, is required by every
// command handler except cmd_help() for file I/O and serialization/
// deserialization.

#include <stdbool.h>

#include "common.h"

// Opens the file specified by filename, reads and deserializes the content
// into a struct sect*, and calls handler by the struct sect* and a user-
// specified void pointer (intended to store command arguments).
// The argument dump specifies whether to dump the struct sect* back at the
// file. For and only for commands that mutate the struct sect* should this
// argument be true.
enum err process(char *filename, enum err (*handler)(struct sect *rec, void *userdata), void *userdata, bool dump);

#endif
