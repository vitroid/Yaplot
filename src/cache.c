/*cache型配列を作る。全要素はつねにmax個で固定。cacheに含まれない要素 */
/*をアクセスしたら、最も旧い要素を削除して場所を提供する。先読みはとり */
/*あえずしないこととする。(先読みさせるなら、destructorだけでなく、デー */
/*タを追加するための関数も必要だし、どのような戦略で読めばいいかわから */
/*ない。)*/

/*非常に大きなキャッシュを作る場合には、このプログラムのように前方から */
/*順番に探索する方法では時間がかかりすぎる。hashと組みあわせるようなこ */
/*とも必要かも。*/
#include "cache.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Cache *Cache_Init(int n, void (*destructor)(void *))
{
	Cache *c;
	c = malloc(sizeof(Cache));
	c->destructor = destructor;
	c->n = n;
	c->index = malloc(sizeof(int) * n);
	c->ptr = malloc(sizeof(void *) * n);
	memset(c->index, -1, sizeof(int) * n);
	memset(c->ptr, (intptr_t)NULL, sizeof(void *) * n);
	return c;
}

void **Cache_ptr(Cache *c, int i)
{
	int j, k;
	for (j = 0; j < c->n; j++) {
		if (c->index[j] == i) {
			void *p = c->ptr[j];
#ifdef DEBUG
			fprintf(stderr, "Cache: found at %d\n", j);
#endif
			for (k = j; k > 0; k--) {
				c->index[k] = c->index[k - 1];
				c->ptr[k] = c->ptr[k - 1];
			}
			c->index[0] = i;
			c->ptr[0] = p;
			return c->ptr;
		}
	}
#ifdef DEBUG
	fprintf(stderr, "Cache: Not found\n");
#endif
	j--;
	c->destructor(c->ptr[j]);
	for (k = j; k > 0; k--) {
		c->index[k] = c->index[k - 1];
		c->ptr[k] = c->ptr[k - 1];
	}
	c->index[0] = i;
	c->ptr[0] = NULL;
	return c->ptr;
}

void Cache_print(Cache *c)
{
	int i;
	for (i = 0; i < c->n; i++) {
		printf("%d %lx\n", c->index[i], (intptr_t)(c->ptr[i]));
	}
	printf("\n");
}

/*
void destructor(const void *v)
{
    char *c;
    if(v!=NULL)
      {
	  c=v;
	  printf("%s freed\n",c);
	  free(v);
      }
}

int main(int argc,char *argv[])
{
    Cache *c;
    char **i;
    c = Cache_Init(3,destructor);
    Cache_print(c);
    i = Cache_ptr(c,1);
    if((*i)==NULL)
      {
	  (*i)=malloc(sizeof(char)*10);
	  strcpy((*i),"alloc 1");
      }
    Cache_print(c);
    i = Cache_ptr(c,4);
    if((*i)==NULL)
      {
	  (*i)=malloc(sizeof(char)*10);
	  strcpy((*i),"alloc 4");
      }
    Cache_print(c);
    i = Cache_ptr(c,1);
    if((*i)==NULL)
      {
	  (*i)=malloc(sizeof(char)*10);
	  strcpy((*i),"again 1");
      }
    Cache_print(c);
    i = Cache_ptr(c,3);
    if((*i)==NULL)
      {
	  (*i)=malloc(sizeof(char)*10);
	  strcpy((*i),"alloc 3");
      }
    Cache_print(c);
    i = Cache_ptr(c,2);
    if((*i)==NULL)
      {
	  (*i)=malloc(sizeof(char)*10);
	  strcpy((*i),"alloc 2");
      }
    Cache_print(c);
}


*/
