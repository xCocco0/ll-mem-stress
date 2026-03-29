
tests=1 2 3 4 P1 P2 P3 P4

outputs=$(addprefix main,${tests})

CFLAGS+= -O3 -fno-tree-vectorize -funroll-all-loops

cdefs=

ifndef BITS
all: ${outputs}
else
cdefs+= -D BITS=${BITS}
all: main
endif

ifdef BASE
cdefs+= -D BASE=${BASE}
endif

ifdef OFFSET
cdefs+= -D OFFSET=${OFFSET}
endif

ifdef DEPTH
cdefs+= -D DEPTH=${DEPTH}
endif

ifdef MEM_PRL
cdefs+= -D MEM_PRL=${MEM_PRL}
endif

ifdef USE_STORE
cdefs+= -D USE_STORE
endif


${outputs}: main.c
	gcc -o $@ -DTEST$(subst main,,$@) $^

main: main.c
	gcc -o $@ ${CFLAGS} ${cdefs} $^

clean:
	rm -f ${outputs} main

.PHONY: clear
