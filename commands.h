#ifndef TCEL_COMMAND_H_
#define TCEL_COMMAND_H_

#include "common.h"

enum cmd {
	CMD_HELP,
	CMD_SHOW, CMD_BRIEF,
	CMD_FMT,
	CMD_DONE, CMD_PART, CMD_SKIP, CMD_PLAN, CMD_PEND,
	CMD_NVAR
};

extern enum cmd_err (*cmds[CMD_NVAR])(int argc, char **argv);

enum cmd_err cmd_help(int argc, char **argv);

enum cmd_err cmd_show(int argc, char **argv);
enum cmd_err cmd_brief(int argc, char **argv);

enum cmd_err cmd_fmt(int argc, char **argv);

enum cmd_err cmd_add(int argc, char **argv);
enum cmd_err cmd_remove(int argc, char **argv);
enum cmd_err cmd_rename(int argc, char **argv);

enum cmd_err cmd_done(int argc, char **argv);
enum cmd_err cmd_part(int argc, char **argv);
enum cmd_err cmd_skip(int argc, char **argv);
enum cmd_err cmd_plan(int argc, char **argv);
enum cmd_err cmd_pend(int argc, char **argv);

#endif
