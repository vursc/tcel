#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TASK_BUF_INIT_SZ = 8;
static int LINE_BUF_SZ = 8192;
static char SECT_LEADER_PREF[] = ": ";
static char SUBSECT_LEADER_PREF[] = ":: ";
static char SLICE_PREF[] = "/";
static int FMT_LINE_SZ = 50;
static int FMT_GROUP_SZ = 5;

static enum err subsect_add_task(struct subsect *ss, enum state task);
static void counter_add_task(struct counter *cntr, enum state task);
static enum err load_record_detail(FILE *fin, struct sect **sl);
static void dump_record_detail(FILE *fout, struct sect *sl);
static void dump_tasks(FILE *fout, unsigned int size, enum state *tasks);

// EXPORTED
enum state char_to_state(char c) {
	switch (c) {
	case '#':
		return STATE_DONE;
	case '=':
		return STATE_PART;
	case '>':
		return STATE_SKIP;
	case '+':
		return STATE_PLAN;
	case '-':
		return STATE_PEND;
	default:
		return STATE_NVAR;
	}
}
char state_to_char(enum state st) {
	switch (st) {
	case STATE_DONE:
		return '#';
	case STATE_PART:
		return '=';
	case STATE_SKIP:
		return '>';
	case STATE_PLAN:
		return '+';
	case STATE_PEND:
		return '-';
	default:
		// impossible
		return '\0';
	}
}

// EXPORTED
struct sect *new_sect() {
	struct sect *result = malloc(sizeof(*result));
	if (result == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		goto err;
	}
	result->head = new_subsect();
	if (result->head == NULL) goto err_free_sect;
	result->cntr = new_counter();
	if (result->cntr == NULL) goto err_free_subsect;
	result->next = NULL;
	return result;

err_free_subsect:
	free(result->head);
err_free_sect:
	free(result);
err:
	fprintf(stderr, "Section node creation failed\n");
	return NULL;
}
struct subsect *new_subsect() {
	struct subsect *result = malloc(sizeof(*result));
	if (result == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		goto err;
	}
	result->size = 0;
	result->capacity = TASK_BUF_INIT_SZ;
	result->tasks = malloc(sizeof(result->tasks[0]) * (result->capacity));
	if (result->tasks == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		goto err_free_result;
	}
	result->cntr = new_counter();
	if (result->cntr == NULL) goto err_free_tasks;
	result->next = NULL;
	result->name = NULL;
	return result;

err_free_tasks:
	free(result->tasks);
err_free_result:
	free(result);
err:
	fprintf(stderr, "Subsection node creation failed\n");
	return NULL;
}
struct counter *new_counter() {
	struct counter *result = malloc(sizeof(*result));
	if (result == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		goto err;
	}
	result->total = 0;
	for (enum state s = STATE_DONE; s < STATE_NVAR; ++s)
		result->cnts[s] = 0;
	return result;

err:
	fprintf(stderr, "Counter creation failed\n");
	return NULL;
}

// EXPORTED
void free_record(struct sect *head) {
	while (head != NULL) {
		struct sect *next = head->next;
		free_sect(head);
		head = next;
	}
}
void free_sect(struct sect *s) {
	struct subsect *head = s->head;
	while (head != NULL) {
		struct subsect *next = head->next;
		free_subsect(head);
		head = next;
	}
	free(s->cntr);
}
void free_subsect(struct subsect *ss) {
	if (ss->name != NULL) free(ss->name);
	free(ss->tasks);
	free(ss->cntr);
	free(ss);
}

// EXPORTED
struct sect *find_sect(struct sect *sl, unsigned int si) {
	struct sect *result = sl;

	unsigned int i = 0;
	while (result != NULL) {
		if (i == si) return result;
		++i;
		result = result->next;
	}

	fprintf(stderr, "Section index out of range\n");
	return NULL;
}
struct subsect *find_subsect(struct subsect *ssl, unsigned int ssi) {
	struct subsect *result = ssl;

	unsigned int i = 0;
	while (result != NULL) {
		if (i == ssi) return result;
		++i;
		result = result->next;
	}

	fprintf(stderr, "Subsection index out of range\n");
	return NULL;
}

// EXPORTED
enum err load_record(char *filename, struct sect **sl) {
	FILE *fin = fopen(filename, "r");
	if (fin == NULL) {
		fprintf(stderr, "Open file %s failed\n", filename);
		return ERR_ERR;
	}
	enum err err = load_record_detail(fin, sl);
	fclose(fin);
	return err;
}
enum err dump_record(char *filename, struct sect *sl) {
	FILE *fout = fopen(filename, "w");
	if (fout == NULL) {
		fprintf(stderr, "Open file %s failed\n", filename);
		return ERR_ERR;
	}
	dump_record_detail(fout, sl);
	fclose(fout);
	return ERR_OK;
}

enum err subsect_add_task(struct subsect *ss, enum state task) {
	if (ss->size == ss->capacity) {
		unsigned int new_capacity = ss->capacity * 2;
		enum state *new_tasks = realloc(ss->tasks,
			sizeof(enum state) * new_capacity);
		if (new_tasks == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			return ERR_ERR;
		}
		ss->capacity = new_capacity;
		ss->tasks = new_tasks;
	}
	ss->tasks[ss->size] = task;
	++ss->size;
	return ERR_OK;
}

void counter_add_task(struct counter *cntr, enum state task) {
	++cntr->total;
	++cntr->cnts[task];
}

// FIXME: Simplify
enum err load_record_detail(FILE *fin, struct sect **sl) {
	struct sect *result = NULL;
	struct sect *curr_sect = NULL;
	struct subsect *curr_subsect = NULL;

	for (unsigned int line_idx = 1; ; ++line_idx) {
		char line[LINE_BUF_SZ];
		if (fgets(line, sizeof(line), fin) == NULL) {
			if (feof(fin)) break;
			fprintf(stderr, "Read line failed\n");
			goto err;
		}
		size_t len = strlen(line);
		if (line[len - 1] != '\n') {
			fprintf(stderr, "Line length limit exceeded\n");
			goto err;
		}
		line[len - 1] = '\0';
		--len;

		if (strncmp(line, SECT_LEADER_PREF, strlen(SECT_LEADER_PREF)) == 0) {
			struct sect *next_sect = new_sect();
			if (next_sect == NULL) goto err;
			next_sect->head->name = malloc(len - strlen(SECT_LEADER_PREF) + 1);
			if (next_sect->head->name == NULL) {
				free_sect(next_sect);
				fprintf(stderr, "Memory allocation failed\n");
				goto err;
			}
			strcpy(next_sect->head->name, line + strlen(SECT_LEADER_PREF));
			if (result == NULL) result = next_sect;
			else curr_sect->next = next_sect;
			curr_sect = next_sect;
			curr_subsect = next_sect->head;
		} else if (strncmp(line, SUBSECT_LEADER_PREF, strlen(SUBSECT_LEADER_PREF)) == 0) {
			if (result == NULL) {
				fprintf(stderr, "Subsection leader outside section\n");
				goto err;
			}
			struct subsect *next_subsect = new_subsect();
			if (next_subsect == NULL) goto err;
			next_subsect->name = malloc(len - strlen(SUBSECT_LEADER_PREF) + 1);
			if (next_subsect->name == NULL) {
				free_subsect(next_subsect);
				fprintf(stderr, "Memory allocation failed\n");
				goto err;
			}
			strcpy(next_subsect->name, line + strlen(SUBSECT_LEADER_PREF));
			curr_subsect->next = next_subsect;
			curr_subsect = next_subsect;
		} else if (strncmp(line, SLICE_PREF, strlen(SLICE_PREF)) == 0) {
			if (result == NULL) {
				fprintf(stderr, "Slice outside section\n");
				goto err;
			}
			struct subsect *next_subsect = new_subsect();
			if (next_subsect == NULL) goto err;
			curr_subsect->next = next_subsect;
			curr_subsect = next_subsect;
			for (size_t i = strlen(SLICE_PREF); i < len; ++i) {
				enum state task = char_to_state(line[i]);
				if (task != STATE_NVAR) {
					if (subsect_add_task(curr_subsect, task) != ERR_OK)
						goto err;
					counter_add_task(curr_subsect->cntr, task);
					counter_add_task(curr_sect->cntr, task);
				}
			}
		} else {
			if (result == NULL) {
				fprintf(stderr, "Task list outside section or subsection\n");
				goto err;
			}
			for (size_t i = 0; i < len; ++i) {
				enum state task = char_to_state(line[i]);
				if (task != STATE_NVAR) {
					if (subsect_add_task(curr_subsect, task) != ERR_OK)
						goto err;
					counter_add_task(curr_subsect->cntr, task);
					counter_add_task(curr_sect->cntr, task);
				}
			}
		}
		continue;

	err:
		free_record(result);
		fprintf(stderr, "Processing line %u failed\n", line_idx);
		fprintf(stderr, "Load record file failed\n");
		return ERR_ERR;
	}
	*sl = result;
	return ERR_OK;
}

void dump_record_detail(FILE *fout, struct sect *sl) {
	for (struct sect *s = sl; s != NULL; s = s->next) {
		fprintf(fout, "%s%s\n", SECT_LEADER_PREF, s->head->name);
		if (s->head->cntr->total > 0) {
			fputs("    ", fout);
			dump_tasks(fout, s->head->cntr->total, s->head->tasks);
		}
		for (struct subsect *ss = s->head->next; ss != NULL; ss = ss->next) {
			if (ss->name == NULL) {
				if (ss->cntr->total > 0) {
					fprintf(fout, "%s%*s", SLICE_PREF, 4 - strlen(SLICE_PREF), "");
					dump_tasks(fout, ss->cntr->total, ss->tasks);
				} else fprintf(fout, "%s\n", SLICE_PREF);
			} else {
				fprintf(fout, "%s%s\n", SUBSECT_LEADER_PREF, ss->name);
				if (ss->cntr->total > 0) {
					fputs("    ", fout);
					dump_tasks(fout, ss->cntr->total, ss->tasks);
				}
			}
		}
		if (s->next != NULL) fputc('\n', fout);
	}
}

void dump_tasks(FILE *fout, unsigned int size, enum state *tasks) {
	while (size > 0) {
		unsigned int batch_sz = size < FMT_LINE_SZ ? size : FMT_LINE_SZ;
		for (unsigned int i = 0; i < batch_sz; ++i) {
			fputc(state_to_char(tasks[i]), fout);
			if ((i + 1) % FMT_GROUP_SZ == 0 && i != batch_sz - 1)
				fputc(' ', fout);
		}
		fputc('\n', fout);

		size = size - batch_sz;
		tasks = tasks + batch_sz;

		if (size > 0) fputs("    ", fout);
	}
}
