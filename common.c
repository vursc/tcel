#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TASK_BUF_INIT_SZ = 8;

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
	for (enum state s = STATE_DONE; s < STATE_NVAR; ++s) {
		result->cnts[s] = 0;
	}
	return result;

err:
	fprintf(stderr, "Counter creation failed\n");
	return NULL;
}

// EXPORTED
void free_record(struct sect *rec) {
	while (rec != NULL) {
		struct sect *next = rec->next;
		free_sect(rec);
		rec = next;
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
	if (ss->name != NULL) {
		free(ss->name);
	}
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
