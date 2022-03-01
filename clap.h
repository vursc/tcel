#ifndef TCEL_CLAP_H_
#define TCEL_CLAP_H_

// Fucntions declared here handles command-line argument parsing.
// These functions are defined in clap.c.

#include <stdbool.h>

#include "cmds.h"

enum cmd parse_command(int argc, char **argv);

bool parse_flag(int argc, char **argv, char *flag, bool fallback);
bool parse_simple_flag(int argc, char **argv, char *flag);

char *parse_filename(int argc, char **argv);
enum err parse_sect_spec(int argc, char **argv, unsigned int *si);
enum err parse_subsect_spec(int argc, char **argv, unsigned int *si, unsigned int *ssi);
enum err parse_task_spec(int argc, char **argv, unsigned int *si, unsigned int *ssi, unsigned int *from, unsigned int *to);

// Ensures that argv is empty (i.e. argv[k][0] == '\0' for k>=1).
// Messages if there is at least one unparsed argument.
enum err parse_done(int argc, char **argv);

#endif
