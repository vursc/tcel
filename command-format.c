#include "commands.h"

#include <stdio.h>

#include "parsers.h"

// EXPORTED
enum cmd_err cmd_fmt(int argc, char **argv) {
	enum cmd_err result = CMD_ERR_OK;
	char *filename = parse_filename(argc, argv);
	if (parse_done(argc, argv) != ERR_OK) {
		result = CMD_ERR_CLA;
		goto err;
	}

	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) {
		result = CMD_ERR_EXEC;
		goto err;
	}
	dump_record(filename, rec);
	free_record(rec);
	return result;

err:
	fprintf(stderr, "Command format failed\n");
	return result;
}