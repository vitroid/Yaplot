#ifndef _HASH_H
#define _HASH_H
#include <stdlib.h>
typedef struct {
	int nentry;
	int hashsize;
	void **value;
	int (*_hash_compare)(const void *, const void *);
} sHash;

int Hash_QueryElement(sHash *h, unsigned int key, void *element);
int Hash_RegisterValue(sHash *h, unsigned int key, void *element);
sHash *Hash_Init(int hashsize, int (*compar)(const void *, const void *));
void Hash_Done(sHash *h);

#endif
