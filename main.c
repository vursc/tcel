#include <stdio.h>
#include <stdlib.h>

#include "clap.h"
#include "cmds.h"
#include "common.h"

int main(int argc, char **argv) {
	enum cmd cmd = parse_command(argc, argv);

	enum cmd_err err = cmds[cmd](argc, argv);
	if (err != CMD_ERR_OK) {
		if (err == CMD_ERR_CLA) {
			puts("Try 'tcel help' for more information");
			putchar('\n');
		}
		return EXIT_FAILURE;
	} else return EXIT_SUCCESS;
}
