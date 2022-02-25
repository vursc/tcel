# TaskCell (tcel)

This is tcel, a command-line utility to manage celluar tasks. By 'celluar' I mean that the tasks are huge in quantity and there is not (much) relationship between. Some examples of celluar tasks are exercises from a textbook and volumes of a comic series. Traditional (to-do list-like) task managers generally do not handle them well, so I created tcel.

Tcel stores task information in a simple, human-readable plain-text format:
- `: <section name>` starts a section,
- `:: <subsection name>` starts a subsection,
- `/<tasks>` starts a slice (anonymous subsect),
- `<tasks>` represents task states. The five characters `#=>+-` represent done, partially done, skipped, planned and pending, respectively.

Several commands are provided by tcel:
- `show` and `brief` to show the content of the record file,
- `format` to format the record file,
- `done`, `part`, `skip`, `plan`, `pend` to alter task state.

Originally I decided that commands that operates on sections and subsections should not be provided, but as my record file grew to over a thousand lines, editing it with a text editor directly is painful because it is always hard to locate sections and subsections. Now that these commands are planned, namely `add`, `remove`, `rename` and `move`. A `append` command may be also added.

An alternative binary storage format is under consideration.

## Coding conventions

Variable naming conventions are:
- `struct sect *rec` (for the record),
- `struct sect *s` (for a single sect) and `struct sect *sl` (for a list of sects),
- `struct susbect *ss` (for a single subsect) and `struct subsect *ssl` (for a list of subsects),
- `struct counter *cntr`,
- `enum state st`.
