#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "sym.h"

#define SLOTS 8
uint16_t slots = SLOTS;
char pool[256] = {0};
uint16_t *table = 0;
uint16_t load = 0;
uint16_t cursor = 0;

uint16_t getload() {
	return load;
}

uint16_t addsym(uint16_t hash, uint16_t loc, uint16_t end) {
	table[loc << 1] = hash;
	table[(loc << 1) + 1] = cursor;
	hash = cursor;
	cursor = end;
	load++;
	return hash;
}

uint16_t putsym(uint16_t hash, uint16_t loc, uint16_t val) {
	table[loc << 1] = hash;
	table[(loc << 1)+1] = val;
	return val;
}

uint16_t intern_n(uint16_t hash, uint16_t val, uint16_t (*put)(uint16_t, uint16_t, uint16_t)) {
	uint16_t start = hash & (slots - 1);
	uint16_t ix = start;

	uint8_t open = 0;
	uint16_t tomb;

	uint16_t thash;
	uint16_t tval;
	while(1) {
		thash = table[ix << 1];
		tval = table[(ix << 1) + 1];
		if (!thash) {
			if(open) ix = tomb;
			return put(hash, ix, val);
		}

		if (!open && !(thash & 0x4000)) {
			open = 1;
			tomb = ix;
		}
		
		if (thash == hash && !strcmp(pool+cursor, pool + tval)) return tval;
		ix++;
		ix &= slots-1;

		if (ix == start) {
			if (!open) return 0xFFFF;
			return put(hash, tomb, val);
		}

	}
	return 0xFFFF;
}

void resize(uint16_t nslots) {
	table = realloc(table, nslots << 2);
}

void collect(uint16_t *buf) {
	uint16_t *i = table;
	uint16_t *end = table+(slots<<1);
	while(i < end) {
		if((*i & 0xc000) != 0xc000) {
			*(i++) = 0;
			*(i++) = 0;
			continue;
		}
printf("c:%4x:%4d: %s\n", *i, *(i+1), pool+*(i+1));
		*(buf++) = *i;
		*(i++) = 0;
		*(buf++) = *i;
		*(i++) = 0;
	}
}

void disburse(uint16_t *buf, uint16_t *end) {
printf("D:%x:%x:%d\n", buf, end, end-buf);
	while(buf < end) {
printf("d:%4x:%4d: %s\n", *buf, *(buf+1), pool+*(buf+1));
		intern_n(*buf,*(buf+1), putsym);
		buf += 2;
	}
}

void zero(uint16_t *i, uint16_t *end) {
	while(i < end) *(i++) = 0;
}

uint16_t loadfactor() {
	return (load * 100) / slots;
}

void grow() {
	uint16_t nslots = slots << 1;
	resize(nslots);
	uint16_t *buf = malloc(load << 2);
	collect(buf);
	zero(table+(slots<<1),table+(nslots<<1));
	slots = nslots;
	disburse(buf, buf+(load<<1));
	free(buf);
}

void rmpool(uint16_t sym) {
	pool[sym] = 0;
}

void shrink() {
	uint16_t nslots = slots >> 1;
	uint16_t *buf = malloc(load << 2);
	collect(buf);
	resize(nslots);
	slots = nslots;
	disburse(buf, buf+(load<<1));
	free(buf);
}

void init() {
	table = malloc(slots<<2);
}

uint16_t hashstep(uint16_t h, char c) {
	h ^= h>>9;
	h *= 0xbeef;
	c ^= c>>3;
	h ^= c;
	return h;
}

uint16_t intern(char *s) {
	uint16_t end = cursor;
	uint16_t hash = 0xdead;
	while(*s) {
		hash = hashstep(hash, *s);
		pool[end++] = *(s++);
	}
	pool[end++] = 0;
	hash |= 0xc000;
	if(loadfactor() > GROW) {
		//this is ugly, but we need the original load for grow to work properly...
		grow();
	}

	return intern_n(hash, end, addsym);
}

void delete(uint16_t sym) {
	uint16_t hash = 0xdead;
	char *s = pool + sym;
	while(*s) hash = hashstep(hash, *(s++)); hash |= 0xc000;
	uint16_t start = hash & (slots-1);
	uint16_t ix = start;
	uint16_t thash;
	uint16_t tval;
	while(1) {
		thash = table[ix << 1];
		tval = table[(ix << 1) + 1];
		if(!thash) return;
		if (thash == hash && !strcmp(pool+sym, pool+tval)) {
			rmpool(sym);
			table[ix << 1] &= 0xbfff;
			load--;
			if(loadfactor() < SHRINK && slots > 8) shrink();
			return;
		}
		ix++;
		ix &= slots-1;
		if (ix == start) return;
	}
}

void printtbl() {
	uint16_t hash;
	uint16_t val;
	for(uint16_t i = 0; i < slots; i++) {
		hash = table[i << 1];
		val = table[(i << 1) + 1];
		printf("%2d:%4x %4d: ", i, hash, val);
		if(!hash) {
			printf("\n");
			continue;
		}
		printf("%s\n", pool+val);
	}
}

char *symstr(uint16_t sym) {
	return pool+sym;
}
