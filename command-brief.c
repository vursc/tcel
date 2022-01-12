#include "commands.h"

#include <stdio.h>
#include <stdlib.h>

#include "parsers.h"

static enum err cmd_brief_all(char *filename, bool compact);
static enum err cmd_brief_sect(char *filename, unsigned int si, bool compact);
static void counter_add(struct counter *dest, struct counter *src);
static void print_header();
static void print_sep();
static void brief_sect(struct sect *s, unsigned int si);
static void brief_subsects(struct sect *s, unsigned int si);
static void brief_counter(struct counter *cntr);

// EXPORTED
enum cmd_err cmd_brief(int argc, char **argv) {
	enum cmd_err result = CMD_ERR_OK;
	bool cla_compact = parse_simple_flag(argc, argv, "compact");
	char *filename = parse_filename(argc, argv);
	unsigned int cla_si;
	if (parse_sect_spec(argc, argv, &cla_si) == ERR_OK) {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (cmd_brief_sect(filename, cla_si, cla_compact) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	} else {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (cmd_brief_all(filename, cla_compact) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	}
	return result;

err:
	fprintf(stderr, "Command brief failed\n");
	return result;
}

enum err cmd_brief_all(char *filename, bool compact) {
	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) return ERR_ERR;
	struct counter *cntr = new_counter();
	if (cntr == NULL) {
		free_record(rec);
		return ERR_ERR;
	}

	print_header();

	struct sect *s = rec;
	unsigned int si = 0;
	while (s != NULL) {
		counter_add(cntr, s->cntr);
		brief_sect(s, si);
		if (!compact) {
			brief_subsects(s, si);
			print_sep();
		}
		s = s->next;
		++si;
	}

	if (compact) print_sep();
	putchar('\n');
	printf("%u tasks, %u done, %u part, %u skip, %u plan, %u pend\n",
		cntr->total, cntr->cnts[STATE_DONE],
		cntr->cnts[STATE_PART], cntr->cnts[STATE_SKIP],
		cntr->cnts[STATE_PLAN], cntr->cnts[STATE_PEND]);
	putchar('\n');

	free_record(rec);
	return ERR_OK;
}

enum err cmd_brief_sect(char *filename, unsigned int si, bool compact) {
	struct sect *rec;
	if (load_record(filename, &rec) != ERR_OK) return ERR_ERR;
	struct sect *sect = find_sect(rec, si);
	if (sect == NULL) {
		free_record(rec);
		return ERR_ERR;
	}
	print_header();
	brief_sect(sect, si);
	if (!compact) brief_subsects(sect, si);
	print_sep();
	putchar('\n');

	free_record(rec);
	return ERR_OK;
}

void counter_add(struct counter *dest, struct counter *src) {
	dest->total = dest->total + src->total;
	for (enum state st = STATE_DONE; st < STATE_NVAR; ++st)
		dest->cnts[st] = dest->cnts[st] + src->cnts[st];
}

void print_header() {
	puts("======= ===== ===================== ==============================");
	puts("   #    total         counts                     name");
	puts("======= ===== ===================== ==============================");
}

void print_sep() {
	puts("------- ----- --------------------- ------------------------------");
}

void brief_sect(struct sect *s, unsigned int si) {
	printf("%3u     ", si + 1);
	brief_counter(s->cntr);
	printf(" %s\n", s->head->name);
}

void brief_subsects(struct sect *s, unsigned int si) {
	// special test for the head subsect
	if (s->head->cntr->total > 0 && s->head->next != NULL) {
		printf("%3u.0   ", si + 1);
		brief_counter(s->head->cntr);
		putchar('\n');
	}

	struct subsect *ss = s->head->next;
	unsigned int ssi = 1;
	while (ss != NULL) {
		printf("%3u.%-3u ", si + 1, ssi);
		brief_counter(ss->cntr);
		if (ss->name != NULL) printf("     %s", ss->name);
		putchar('\n');
		ss = ss->next;
		++ssi;
	}
}

void brief_counter(struct counter *cntr) {
	printf("%5u  ", cntr->total);
	for (enum state st = STATE_DONE; st < STATE_NVAR; ++st) {
		if (cntr->cnts[st] > 0) printf("%3u ", cntr->cnts[st]);
		else printf("  . ");
	}
}
