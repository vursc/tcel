#include "clap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum cmd parse_command(int argc, char **argv) {
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		char *cmd_str[] = {
			[CMD_HELP] = "help",
			[CMD_SHOW] = "show",
			[CMD_BRIEF] = "brief",
			[CMD_FMT] = "format",
			[CMD_DONE] = "done",
			[CMD_PART] = "part",
			[CMD_SKIP] = "skip",
			[CMD_PLAN] = "plan",
			[CMD_PEND] = "pend",
		};
		for (enum cmd c = CMD_HELP; c < CMD_NVAR; ++c) {
			if (strcmp(argv[i], cmd_str[c]) == 0) {
				argv[i][0] = '\0';
				return c;
			}
		}
	}
	return CMD_SHOW;
}

bool parse_flag(int argc, char **argv, char *flag, bool fallback) {
	bool result = fallback;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		if (strncmp(argv[i], "--", strlen("--")) == 0 && strcmp(argv[i] + strlen("--"), flag) == 0) {
			argv[i][0] = '\0';
			result = true;
		} else if (strncmp(argv[i], "--no-", strlen("--no-")) == 0 && strcmp(argv[i] + strlen("--no-"), flag) == 0) {
			argv[i][0] = '\0';
			result = false;
		}
	}
	return result;
}

bool parse_simple_flag(int argc, char **argv, char *flag) {
	bool result = false;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		if (strncmp(argv[i], "--", strlen("--")) == 0 && strcmp(argv[i] + strlen("--"), flag) == 0) {
			argv[i][0] = '\0';
			result = true;
		}
	}
	return result;
}

char *parse_filename(int argc, char **argv) {
	char *p = NULL;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		if (strncmp(argv[i], "-r", strlen("-r")) == 0) {
			p = argv[i] + strlen("-r");
			argv[i][0] = '\0';
		}
	}
	if (p != NULL) {
		size_t len = strlen(p);
		char *str = malloc(len + sizeof(".cell"));
		if (str == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			// FIXME: Handle this more gracefully
			// Remember to remove #include <stdlib.h>
			abort();
		}
		memcpy(str, p, len);
		memcpy(str + len, ".cell", sizeof(".cell"));
		return str;
	} else return "tasks.cell";
}

enum err parse_sect_spec(int argc, char **argv, unsigned int *si) {
	unsigned int si_;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		int pos;
		if (sscanf(argv[i], "%u%n", &si_, &pos) == 1 && argv[i][pos] == '\0') {
			*si = si_ - 1;
			argv[i][0] = '\0';
			return ERR_OK;
		}
	}
	return ERR_ERR;
}

enum err parse_subsect_spec(int argc, char **argv, unsigned int *si, unsigned int *ssi) {
	unsigned int si_, ssi_;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		int pos;
		if (sscanf(argv[i], "%u.%u%n", &si_, &ssi_, &pos) == 2 && argv[i][pos] == '\0') {
			*si = si_ - 1;
			*ssi = ssi_;
			argv[i][0] = '\0';
			return ERR_OK;
		}
	}
	return ERR_ERR;
}

enum err parse_task_spec(int argc, char **argv, unsigned int *si, unsigned int *ssi, unsigned int *from, unsigned int *to) {
	unsigned int si_, ssi_, from_, to_;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '\0') continue;
		int pos;
		if (sscanf(argv[i], "%u.%u.%u-%u%n", &si_, &ssi_, &from_, &to_, &pos) == 4
			&& argv[i][pos] == '\0') {
			*si = si_ - 1;
			*ssi = ssi_;
			*from = from_ - 1;
			*to = to_;
			argv[i][0] = '\0';
			return ERR_OK;
		} else if (sscanf(argv[i], "%u.%u-%u%n", &si_, &from_, &to_, &pos) == 3
			&& argv[i][pos] == '\0') {
			*si = si_ - 1;
			*ssi = 0;
			*from = from_ - 1;
			*to = from_;
			argv[i][0] = '\0';
			return ERR_OK;
		} else if (sscanf(argv[i], "%u.%u.%u%n", &si_, &ssi_, &from_, &pos) == 3
			&& argv[i][pos] == '\0') {
			*si = si_ - 1;
			*ssi = ssi_;
			*from = from_ - 1;
			*to = from_;
			argv[i][0] = '\0';
			return ERR_OK;
		} else if (sscanf(argv[i], "%u.%u%n", &si_, &from_, &pos) == 2
			&& argv[i][pos] == '\0') {
			*si = si_ - 1;
			*ssi = 0;
			*from = from_ - 1;
			*to = from_;
			argv[i][0] = '\0';
			return ERR_OK;
		}
	}
	return ERR_ERR;
}

enum err parse_done(int argc, char **argv) {
	enum err result = ERR_OK;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] != '\0') {
			fprintf(stderr, "Unrecognized argument: %s\n", argv[i]);
			argv[i][0] = '\0';
			result = ERR_ERR;
		}
	}
	return result;
}
