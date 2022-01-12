#include "commands.h"

// EXPORTED
enum cmd_err (*cmds[CMD_NVAR])(int argc, char **agv) = {
	[CMD_HELP] = cmd_help,
	[CMD_SHOW] = cmd_show,
	[CMD_BRIEF] = cmd_brief,
	[CMD_FMT] = cmd_fmt,
	[CMD_DONE] = cmd_done,
	[CMD_PART] = cmd_part,
	[CMD_SKIP] = cmd_skip,
	[CMD_PLAN] = cmd_plan,
	[CMD_PEND] = cmd_pend,
};
