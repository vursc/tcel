#include "commands.h"

#include <stdio.h>

#include "parsers.h"

static int DISP_LINE_SZ = 50;
static int DISP_GROUP_SZ = 10;
static char *COLORS[STATE_NVAR] = {
	[STATE_DONE] = "\033[42m",
	[STATE_PART] = "\033[45m",
	[STATE_SKIP] = "\033[44m",
	[STATE_PLAN] = "\033[41m",
	[STATE_PEND] = "\033[43m",
};

static enum err cmd_show_all(char *filename, bool color);
static enum err cmd_show_sect(char *filename, unsigned int si, bool color);
static enum err cmd_show_subsect(char *filename, unsigned int si, unsigned int ssi, bool color);
static void show_sect(struct sect *s, unsigned int si, bool color);
static void show_sect_header(char *name, unsigned int si, bool color);
static void show_subsect(struct subsect *ss, unsigned int si, unsigned int ssi, bool color);
static void show_subsect_header(char *name, unsigned int si, unsigned int ssi, bool color);

// EXPORTED
enum cmd_err cmd_show(int argc, char **argv) {
	enum cmd_err result = CMD_ERR_OK;
	bool cla_color = parse_color(argc, argv);
	char *filename = parse_filename(argc, argv);

	unsigned int cla_si, cla_ssi;
	if (parse_sect_spec(argc, argv, &cla_si) == ERR_OK) {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (cmd_show_sect(filename, cla_si, cla_color) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	} else if (parse_subsect_spec(argc, argv, &cla_si, &cla_ssi) == ERR_OK) {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (cmd_show_subsect(filename, cla_si, cla_ssi, cla_color) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	} else {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (cmd_show_all(filename, cla_color) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	}
	return result;

err:
	fprintf(stderr, "Command show failed\n");
	return result;
}

enum err cmd_show_all(char *filename, bool color) {
	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) return ERR_ERR;

	struct sect *s = rec;
	unsigned int si = 0;
	while (s != NULL) {
		show_sect(s, si, color);

		s = s->next;
		++si;
	}

	putchar('\n');

	free_record(rec);
	return ERR_OK;
}

enum err cmd_show_sect(char *filename, unsigned int si, bool color) {
	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) return ERR_ERR;

	struct sect *s = find_sect(rec, si);
	if (s == NULL) {
		free_record(rec);
		return ERR_ERR;
	}
	show_sect(s, si, color);
	putchar('\n');

	free_record(rec);
	return ERR_OK;
}

enum err cmd_show_subsect(char *filename, unsigned int si, unsigned int ssi, bool color) {
	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) return ERR_ERR;

	struct sect *s = find_sect(rec, si);
	if (s == NULL) goto err_free_rec;

	struct subsect *ss = s->head;
	struct subsect *last_named_ss = NULL;
	unsigned int last_named_ssi = 0;
	// If the ssi-th subsect is the first named subsect,
	// last_named_ss does not necessarily point to it,
	// since it will not be accessed
	for (unsigned int i = 0; i < ssi; ++i) {
		if (ss == NULL) goto err_out_of_range;
		if (ss->name != NULL) {
			last_named_ss = ss;
			last_named_ssi = i;
		}
		ss = ss->next;
	}

	if (ss == NULL) goto err_out_of_range;
	show_sect_header(s->head->name, si, color);
	if (ss->name == NULL && last_named_ss != NULL)
		show_subsect_header(last_named_ss->name, si, last_named_ssi, color);
	show_subsect(ss, si, ssi, color);
	putchar('\n');

	free_record(rec);
	return ERR_OK;

err_out_of_range:
	fprintf(stderr, "Subsection index out of range\n");
err_free_rec:
	free_record(rec);
	return ERR_ERR;
}

void show_sect(struct sect *s, unsigned int si, bool color) {
	show_sect_header(s->head->name, si, color);

	struct subsect *ss = s->head;
	unsigned int ssi = 0;
	while (ss != NULL) {
		show_subsect(ss, si, ssi, color);

		ss = ss->next;
		++ssi;
	}
}

void show_sect_header(char *name, unsigned int si, bool color) {
	if (color) printf("\033[7m[%u]\033[0m %s\n", si + 1, name);
	else printf("[%u] %s\n", si + 1, name);
}

void show_subsect(struct subsect *ss, unsigned int si, unsigned int ssi, bool color) {
	if (ssi != 0) show_subsect_header(ss->name, si, ssi, color);

	unsigned int size = ss->size;
	enum state *tasks = ss->tasks;
	while (size > 0) {
		printf("        ");
		unsigned int batch_sz = size < DISP_LINE_SZ ? size : DISP_LINE_SZ;
		for (unsigned int i = 0; i < batch_sz; ++i) {
			if (i % DISP_GROUP_SZ == 0 && i != 0) {
				if (color) printf("\033[0m");
				putchar(' ');
			}
			if (color) fputs(COLORS[tasks[i]], stdout);
			putchar(state_to_char(tasks[i]));
		}
		printf("\033[0m\n");

		size = size - batch_sz;
		tasks = tasks + batch_sz;
	}
}

void show_subsect_header(char *name, unsigned int si, unsigned int ssi, bool color) {
	if (color) printf("    \033[7m[%u.%u]\033[0m", si + 1, ssi);
	else printf("    [%u.%u]", si + 1, ssi);
	if (name != NULL) printf(" %s", name);
	putchar('\n');
}
