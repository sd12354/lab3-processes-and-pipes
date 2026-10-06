CC = gcc
CFLAGS = -Wall -Wextra

all: pipes_processes3

pipes_processes3: pipes_processes3.c
	$(CC) $(CFLAGS) -o $@ $<

# Compare the program's output with the real shell pipeline
test: pipes_processes3
	@for arg in 28 Lakers 44 .611; do \
		./pipes_processes3 $$arg > .got; \
		cat scores | grep $$arg | sort > .want; \
		if cmp -s .got .want; then echo "PASS: $$arg"; else echo "FAIL: $$arg"; fi; \
	done; rm -f .got .want

clean:
	rm -f pipes_processes3 .got .want

.PHONY: all test clean
