#ifndef TCEL_COMMON_H_
#define TCEL_COMMON_H_

enum err {
	ERR_OK,
	ERR_ERR,
};

enum cmd_err {
	CMD_ERR_OK,
	CMD_ERR_CLA,
	CMD_ERR_EXEC,
};

enum state {
	STATE_DONE,
	STATE_PART,
	STATE_SKIP,
	STATE_PLAN,
	STATE_PEND,

	STATE_NVAR
};

struct sect {
	struct sect *next;
	struct subsect *head;
	struct counter *cntr;
};

struct subsect {
	struct subsect *next;
	char *name;
	unsigned int size, capacity;
	enum state *tasks;
	struct counter *cntr;
};

struct counter {
	unsigned int total;
	unsigned int cnts[STATE_NVAR];
};

enum state char_to_state(char c);
char state_to_char(enum state st);

struct sect *new_sect();
struct subsect *new_subsect();
struct counter *new_counter();
void free_record(struct sect *sl);
void free_sect(struct sect *s);
void free_subsect(struct subsect *ss);

struct sect *find_sect(struct sect *sl, unsigned int si);
struct subsect *find_subsect(struct subsect *ssl, unsigned int ssi);

enum err load_record(char *filename, struct sect **sl);
enum err dump_record(char *filename, struct sect *sl);

#endif
