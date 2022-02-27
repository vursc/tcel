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

Originally I decided that commands that operates on sections and subsections should not be provided, but as my record file grew to over a thousand lines, editing it with a text editor directly is painful because it is always hard to locate sections and subsections. Now that these commands are planned, namely `add`, `remove`, `rename` and `move`. The previously proposed `append` command is replaced by a `resize` command.

The semantics of these new commands are undecided. Currently (all `<*ssi>` cannot be `0`, except in `resize`):
- `add <si> <name> <sz>` adds a section in position `<si>` with name `<name>` and with `<sz>` tasks in initial subsection.
- `add <si>.<ssi> <name> <sz` adds a subsection in position `<si>.<ssi>` with name `<name>` (`/` for anonymous) and with `<sz>` tasks.
- `remove <si>` and `remove <si>.<ssi>` behaves literally. It asks the user to confirm deletion. The `-y` flag skips this.
- `rename <si> <name>` and `rename <si>.<ssi> <name>` behaves literally.
- `move <src_si> <dest_si>` and `move <si> <src_ssi> <dest_ssi>` behaves literally. The program does not support moving subsections between sections (why would anyone want to do that?).
- `resize <si>.<ssi> <sz>` shrinks the subsection to given size, if `<sz>` is smaller than the current size; otherwise expands that subsection to given size, padding extra tasks with `-` (pend).

## Coding conventions

Variable naming conventions are:
- `struct sect *rec` (for the record),
- `struct sect *s` (for a single sect) and `struct sect *sl` (for a list of sects),
- `struct susbect *ss` (for a single subsect) and `struct subsect *ssl` (for a list of subsects),
- `struct counter *cntr`,
- `enum state st`.
