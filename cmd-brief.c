#include "cmds.h"

#include <stdio.h>
#include <stdlib.h>

#include "clap.h"
#include "serde.h"

static enum err cmd_brief_all(struct sect *rec, void *userdata);
static enum err cmd_brief_sect(struct sect *rec, void *userdata);
static void counter_add(struct counter *dest, struct counter *src);
static void print_header();
static void print_sep();
static void brief_sect(struct sect *s, unsigned int si);
static void brief_subsects(struct sect *s, unsigned int si);
static void brief_counter(struct counter *cntr);

struct cmd_brief_arg_buf {
	unsigned int si;
	bool compact;
};

// EXPORTED
enum cmd_err cmd_brief(int argc, char **argv) {
	enum cmd_err result = CMD_ERR_OK;
	char *filename = parse_filename(argc, argv);
	struct cmd_brief_arg_buf buf;
	buf.compact = parse_simple_flag(argc, argv, "compact");
	if (parse_sect_spec(argc, argv, &(buf.si)) == ERR_OK) {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (process(filename, cmd_brief_sect, &buf, false) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	} else {
		if (parse_done(argc, argv) != ERR_OK) {
			result = CMD_ERR_CLA;
			goto err;
		}
		if (process(filename, cmd_brief_all, &buf, false) != ERR_OK) {
			result = CMD_ERR_EXEC;
			goto err;
		}
	}
	return result;

err:
	fprintf(stderr, "Command brief failed\n");
	return result;
}

enum err cmd_brief_all(struct sect *rec, void *userdata) {
	struct cmd_brief_arg_buf *buf = userdata;
	struct counter *cntr = new_counter();
	if (cntr == NULL) return ERR_ERR;
	print_header();
	struct sect *s = rec;
	unsigned int si = 0;
	while (s != NULL) {
		counter_add(cntr, s->cntr);
		brief_sect(s, si);
		if (!(buf->compact)) {
			brief_subsects(s, si);
			print_sep();
		}
		s = s->next;
		++si;
	}
	if (buf->compact) print_sep();
	putchar('\n');
	printf("%u tasks. %u done, %u part, %u skip, %u plan, %u pend.\n",
		cntr->total, cntr->cnts[STATE_DONE],
		cntr->cnts[STATE_PART], cntr->cnts[STATE_SKIP],
		cntr->cnts[STATE_PLAN], cntr->cnts[STATE_PEND]);
	putchar('\n');
	return ERR_OK;
}

enum err cmd_brief_sect(struct sect *rec, void *userdata) {
	struct cmd_brief_arg_buf *buf = userdata;
	struct sect *sect = find_sect(rec, buf->si);
	if (sect == NULL) return ERR_ERR;
	print_header();
	brief_sect(sect, buf->si);
	if (!(buf->compact)) brief_subsects(sect, buf->si);
	print_sep();
	putchar('\n');
	return ERR_OK;
}

void counter_add(struct counter *dest, struct counter *src) {
	dest->total = dest->total + src->total;
	for (enum state st = STATE_DONE; st < STATE_NVAR; ++st)
		dest->cnts[st] = dest->cnts[st] + src->cnts[st];
}

void print_header() {
	puts("======= ======= ========================== ==============================");
	puts("   #     total            counts                        name");
	puts("======= ======= ========================== ==============================");
}

void print_sep() {
	puts("------- ------- ---------------------------- ------------------------------");
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
		if (ss->name != NULL) printf("      %s", ss->name);
		putchar('\n');
		ss = ss->next;
		++ssi;
	}
}

void brief_counter(struct counter *cntr) {
	printf("%7u  ", cntr->total);
	for (enum state st = STATE_DONE; st < STATE_NVAR; ++st) {
		if (cntr->cnts[st] > 0) printf("%4u ", cntr->cnts[st]);
		else printf("   . ");
	}
}
