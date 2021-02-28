#include <stdio.h>
#include <stdint.h>
#include "sym.h"

int main(int argc, char **argv) {
	init();
	intern("hello");
	uint16_t r = intern("nil");
	intern("map");
	intern("defmacro");
	intern("harrison");
	printtbl();
	delete(r);
	printf("\n");
	printtbl();
	intern("world");
	intern("enter");
	intern("harrison");
	intern("enters");
	intern("fiend");
	intern("friend");
	uint16_t d = intern("fiendish");
	uint16_t a = intern("fiendishly");
	uint16_t b = intern("Camille");
	uint16_t c = intern("Riley");
	printf("\n");
	printtbl();
	delete(d);
	delete(a);
	delete(b);
	delete(c);
	printf("\n");
	printtbl();
	printf("load:%d\n", getload());
}
