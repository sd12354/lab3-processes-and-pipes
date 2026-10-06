# Lab 3: Processes and Pipes

Part 2 of the lab: chaining three processes together with two pipes.

## pipes_processes3.c

Runs the equivalent of the shell command

```
cat scores | grep <argument> | sort
```

using `fork()`, `pipe()`, `dup2()` and `execvp()`. The grep pattern comes from
the command line (`argv[1]`).

| Process | Role | Reads from | Writes to |
| --- | --- | --- | --- |
| P1 (parent) | `cat scores` | the `scores` file | pipe 1 |
| P2 (child) | `grep <argument>` | pipe 1 | pipe 2 |
| P3 (child's child) | `sort` | pipe 2 | the terminal |

Each process closes every pipe file descriptor it does not need before calling
`execvp()`. This matters: `grep` and `sort` only finish when they see
end-of-file on their input, and that only happens once every copy of the
pipe's write end has been closed.

As in the starter program `pipes_processes2.c`, the parent does not wait for
its child, so the shell prompt can reappear before the output finishes.

## Build and run

```
make
./pipes_processes3 28
```

Example output with the lab's `scores` file:

```
Houston 44 28 .611
Indiana 45 28 .616
Oklahoma City 44 28 .611
Utah 44 28 .611
```

Run `make test` to compare the program against the real shell pipeline for a
few patterns.

## Note on the scores file

The `scores` file in this repo is sample data in the same format as the lab's
file (`team wins losses pct`), included so the program runs straight after
cloning. The program must be run from the directory that contains `scores`.
