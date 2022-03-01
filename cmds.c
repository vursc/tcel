#include "cmds.h"

#include <stdio.h>

#include "clap.h"
#include "serde.h"

static enum cmd_err cmd_alter(int argc, char **argv, enum state st);
static enum err cmd_alter_detail(struct sect *rec, void *userdata);

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

// EXPORTED
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
	puts("Task specification formats:");
	puts("    <section>.<task>");
	puts("    <section>.<from>-<to>");
	puts("    <section>.<subsection>.<task>");
	puts("    <section>.<subsection>.<from>-<to>");
	puts("");
	puts("Options:");
	puts("  --compact");
	puts("    Show sections only");
	puts("  --no-color");
	puts("    Disable colored output");
	puts("  -r<file>");
	puts("    Specify record file, without extension .cell [default: -rtasks]");
	puts("");
	return CMD_ERR_OK;
}

// EXPORTED
enum cmd_err cmd_fmt(int argc, char **argv) {
	enum cmd_err result = CMD_ERR_OK;
	char *filename = parse_filename(argc, argv);
	if (parse_done(argc, argv) != ERR_OK) {
		result = CMD_ERR_CLA;
		goto err;
	}
	return process(filename, NULL, NULL, true);

err:
	fprintf(stderr, "Command format failed\n");
	return result;
}

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

struct cmd_alter_arg_buf {
	unsigned int si;
	unsigned int ssi;
	unsigned int from;
	unsigned int to;
	enum state st;
};

enum cmd_err cmd_alter(int argc, char **argv, enum state st) {
	enum cmd_err result = CMD_ERR_OK;
	char *filename = parse_filename(argc, argv);
	struct cmd_alter_arg_buf buf;
	buf.st = st;
	enum err err = parse_task_spec(argc, argv, &(buf.si), &(buf.ssi), &(buf.from), &(buf.to));
	if (parse_done(argc, argv) != ERR_OK) {
		result = CMD_ERR_CLA;
		goto err;
	}
	if (err != ERR_OK) {
		fprintf(stderr, "Task specifier required\n");
		result = CMD_ERR_CLA;
		goto err;
	}
	if (process(filename, cmd_alter_detail, &buf, true) != ERR_OK) {
		result = CMD_ERR_EXEC;
		goto err;
	}
	return result;
err:
	fprintf(stderr, "Command alter failed\n");
	return result;
}

enum err cmd_alter_detail(struct sect *rec, void *userdata) {
	struct cmd_alter_arg_buf *buf = userdata;
	struct sect *s = find_sect(rec, buf->si);
	if (s == NULL) return ERR_ERR;
	struct subsect *ss = find_subsect(s->head, buf->ssi);
	if (ss == NULL) return ERR_ERR;
	if (buf->to > ss->cntr->total) {
		fprintf(stderr, "Task index out of range\n");
		return ERR_ERR;
	}
	for (unsigned int i = buf->from; i < buf->to; ++i) {
		ss->tasks[i] = buf->st;
	}
	return ERR_OK;
}
