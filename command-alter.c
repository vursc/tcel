#include "commands.h"

#include <stdio.h>

#include "parsers.h"

static enum cmd_err cmd_alter(int argc, char **argv, enum state st);
static enum err cmd_alter_detail(char *filename,
	unsigned int si, unsigned int ssi, unsigned int from,
	unsigned int to, enum state st);

// EXPORTED
enum cmd_err cmd_done(int argc, char **argv) {
	return cmd_alter(argc, argv, STATE_DONE);
}
enum cmd_err cmd_part(int argc, char **argv) {
	return cmd_alter(argc, argv, STATE_PART);
}
enum cmd_err cmd_skip(int argc, char **argv) {
	return cmd_alter(argc, argv, STATE_SKIP);
}
enum cmd_err cmd_plan(int argc, char **argv) {
	return cmd_alter(argc, argv, STATE_PLAN);
}
enum cmd_err cmd_pend(int argc, char **argv) {
	return cmd_alter(argc, argv, STATE_PEND);
}

static enum cmd_err cmd_alter(int argc, char **argv, enum state st) {
	enum cmd_err result = CMD_ERR_OK;
	char *filename = parse_filename(argc, argv);
	unsigned int cla_si, cla_ssi, cla_from, cla_to;
	enum err err = parse_task_spec(argc, argv, &cla_si, &cla_ssi, &cla_from, &cla_to);
	if (parse_done(argc, argv) != ERR_OK) {
		result = CMD_ERR_CLA;
		goto err;
	}
	if (err != ERR_OK) {
		fprintf(stderr, "Task specifier required\n");
		result = CMD_ERR_CLA;
		goto err;
	}
	if (cmd_alter_detail(filename, cla_si, cla_ssi, cla_from, cla_to, st) != ERR_OK) {
		result = CMD_ERR_EXEC;
		goto err;
	}
	return result;
err:
	fprintf(stderr, "Command alter failed\n");
	return result;
}

enum err cmd_alter_detail(char *filename, unsigned int si, unsigned int ssi,
	unsigned int from, unsigned int to, enum state st) {
	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) goto err;

	struct sect *s = find_sect(rec, si);
	if (s == NULL) goto err_free_rec;
	struct subsect *ss = find_subsect(s->head, ssi);
	if (ss == NULL) goto err_free_rec;
	if (to > ss->cntr->total) {
		fprintf(stderr, "Task index out of range\n");
		goto err_free_rec;
	}
	for (unsigned int i = from; i < to; ++i)
		ss->tasks[i] = st;

	dump_record(filename, rec);
	free_record(rec);
	return ERR_OK;

err_free_rec:
	free_record(rec);
err:
	return ERR_ERR;
}
