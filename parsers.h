#ifndef TCEL_PARSERS_H_
#define TCEL_PARSERS_H_

#include <stdbool.h>

#include "commands.h"

enum cmd parse_command(int argc, char **argv);
bool parse_flag(int argc, char **argv, char *flag, bool fallback);
bool parse_simple_flag(int argc, char **argv, char *flag);

bool parse_color(int argc, char **argv);

char *parse_filename(int argc, char **argv);
enum err parse_sect_spec(int argc, char **argv, unsigned int *si);
enum err parse_subsect_spec(int argc, char **argv,
	unsigned int *si, unsigned int *ssi);
enum err parse_task_spec(int argc, char **argv,
	unsigned int *si, unsigned int *ssi,
	unsigned int *from, unsigned int *to);

enum err parse_done(int argc, char **argv);

#endif
