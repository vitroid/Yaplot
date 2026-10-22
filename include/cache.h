#ifndef _CACHE_H
#define _CACHE_H
typedef struct {
	int n;
	int *index;
	void **ptr;
	void (*destructor)(void *);
} Cache;

Cache *Cache_Init(int n, void (*destructor)(void *));
void Cache_Destroy(Cache *c);
void **Cache_ptr(Cache *c, int i);

#endif
