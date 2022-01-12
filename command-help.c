#include "commands.h"

#include <stdio.h>

enum cmd_err cmd_help(int argc, char **argv) {
	puts("This is tcel, a celluar task manager");
	puts("");
	puts("Usage:");
	puts("    tcel help");
	puts("    tcel [show] [<section> | <section>.<subsection>] [--no-color] [-r<file>]");
	puts("    tcel brief [<section] [--compact] [-r<file>]");
	puts("    tcel format [-r<file>]");
	puts("    tcel (done | part | skip | plan | pend) <tasks> [-r<file>]");
	puts("");
	puts("Flags:");
	puts("  --compact");
	puts("    Show sections only, omit subsections");
	puts("  --no-color");
	puts("    Disable colored output");
	puts("  -r<file>");
	puts("    Specify record file, without extension .cell [default: -rtasks]");
	puts("");
	return CMD_ERR_OK;
}