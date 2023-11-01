
tests=1 2 3 4 P1 P2 P3 P4

outputs=$(addprefix main,${tests})

ifndef BITS
all: ${outputs}
else
all: main
endif

${outputs}: main.c
	gcc -o $@ -DTEST$(subst main,,$@) $^

main: main.c
	gcc -o $@ -D BITS=${BITS} $^

clean:
	rm -f ${outputs} main

.PHONY: clear
