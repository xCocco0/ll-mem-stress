
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

//#define USE_STORE
//#define AUTO_THROTTLE

#if defined(TEST1)
#define BITS 14,15,4,5,6 //1
#define BASE 0x0
#elif defined(TEST2)
#define BITS 14,15 //2
#define BASE 0x0
#elif defined(TEST3)
#define BITS 16,17,18,19,6,7,8,9,10,11,12,13,14,15 //3
#define BASE 0x0
#elif defined(TEST4)
#define BITS 16,17,18,19,20 //4
#define BASE 0x0
#elif defined(TESTP1)
#define BITS 15,16,4,5,6 //P1
#define BASE 0x0
#elif defined(TESTP2)
#define BITS 15,16 //P2
#define BASE 0x0
#elif defined(TESTP3)
#define BITS 16,17,18,19,20,6,7,8,9,10,11,12,13,15 //P3
#define BASE 0x0
#elif defined(TESTP4)
#define BITS 16,17,18,19,20 //P4
#define BASE 0x0
#else
#ifndef BITS
#warning "No test defined. Using default values"
#define BITS 16,17,7,8,9,10,11,12,13,15
#endif
#ifndef BASE
#define BASE 0x40
#endif
#endif


#define DEPTH 16
#if DEPTH < 8
#error depth too low
#endif

#define MEM_SIZE (1<<21)

//force loop unrolling
#define DO_0(body) { body }
#define DO_1(body) {DO_0(body); DO_0(body)} // 2^1 = 2
#define DO_2(body) {DO_1(body); DO_1(body)} // 2^2 = 4
#define DO_3(body) {DO_2(body); DO_2(body)} // 2^3 = 8
#define DO_4(body) {DO_3(body); DO_3(body)} // 2^4 = 16
#define DO_5(body) {DO_4(body); DO_4(body)} // 2^5 = 32

typedef struct {
	void* next;
	char byte[DEPTH-sizeof(void*)];
} mem_chunk;

// flip bit #bit and return 1 if overflow occured
#define FLIP(addr, bit) !((addr ^= (1 << bit)) & (1 << bit))

static const int bits[] = {BITS};
/*	- - 2 1 - 3 -
 *	0 0 0 0 0 0 0
 *	0 0 0 1 0 0 0
 *	0 0 1 0 0 0 0
 *	0 0 1 1 0 0 0
 *	0 0 0 0 0 1 0
 *	0 0 0 1 0 1 0
 *	0 0 1 0 0 1 0
 * */

void next_offset(unsigned long *offset, int i) {
	//printf("%#d, %ld (%#x), %d\n", base, *offset, *offset, i);
	if(i < sizeof(bits)/sizeof(bits[0]) && FLIP(*offset, bits[i]))
		next_offset(offset, i+1);
}

static inline void copy_lw(char* base, unsigned long from, unsigned long to) {
	*(void**)(base+to) = (void*)(base+from);
}

void prepare_mem(char* base) {
	unsigned long offset = 0, old = 0;
	do {
		old = offset;
		//printf("offset = %0#8x (%lu)\n", offset, offset);
		next_offset(&offset, 0);
		copy_lw(base, old, offset);
	} while(offset);
	copy_lw(base, old, 0);
}

int main(int argc, char **argv)
{
	char* addr;

	addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	if(addr == (char*) -1) {
		fprintf(stderr, "%d ", errno);
		perror("Error mmap");
		exit(1);
	}

	mlockall(MCL_FUTURE);

	printf("Successfully loaded at %p\n", addr);
	printf("Bit mask: %#x\n", 0);
	
	void* start_addr = (void*) ((unsigned long)(addr) + BASE);
	prepare_mem(start_addr);
	
	mem_chunk r;
	register mem_chunk* c = (mem_chunk*) start_addr;
	register int i;
	for(;;) DO_5(
	{
#ifdef AUTO_THROTTLE
		for(i = 0; i < 1<<27; ++i) { // ~1500ms
#endif
#ifdef USE_STORE	
			r = *c;
#endif
			c = *(mem_chunk**)c;
			asm volatile ("" : : "g"(c));
			//printf("0x%x\n", c);
#ifdef AUTO_THROTTLE
		}
		usleep(2500); //let the OS breath 2.5ms/1500ms = 0.167%
#endif
	}
	)
	return 0;
}
/*
	//////////////////

	char b;
	unsigned int offset = 0;
	long diffns;
	struct timespec now, old;
	clock_gettime(CLOCK_MONOTONIC, &old);

	while(1) {
		for(int i = 0; i < ITERATIONS; i++) {
			b = addr[offset];
			offset = (offset + STRIDE) % MSIZE;
		}
		clock_gettime(CLOCK_MONOTONIC, &now);
		diffns = (now.tv_sec - old.tv_sec)*1000000000L + (now.tv_nsec - old.tv_nsec);
		printf("Duration: %.3f us\t Bandwidth: %f GB/s\n",\
			diffns/1000.0, (float)(ITERATIONS)/diffns );
		//old = now;
		usleep(100000);
		clock_gettime(CLOCK_MONOTONIC, &old);
	}

	getc(stdin);

	return 0;
}
*/
