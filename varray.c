#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "varray.h"

varray * varray_init(size_t nmemb,size_t size,int initval)
{
    varray *v;
    v = malloc(sizeof(varray));
    v->initval = initval;
    v->size = size;
    v->n=nmemb;
    v->a=malloc(size*nmemb);
    memset(v->a,initval,size*nmemb);
#ifdef DEBUG
	  fprintf(stderr,"VARRAY: alloc %d\n",v->n);
#endif
    return v;
}

void *varray_ptr(varray *v,int i)
{
    int last;
    char *a;
    last = v->n;
    while(v->n <= i)
      {
	  v->n *= 2;
      }
    if(v->n!=last)
      {
#ifdef DEBUG
	  fprintf(stderr,"VARRAY: realloc %d -> %d\n",last,v->n);
#endif
	  a =  v->a = realloc(v->a,v->n*v->size);
	  a +=last*v->size;
	  memset(a,/*&v->a[last*v->size],*/
		 v->initval,
		 (v->n-last)*v->size);
      }
    a = v->a;
    a += i*v->size;
    return a;
}

void varray_done(varray *v)
{
    free(v->a);
    free(v);
}

/*
void varray_print(varray *v)
{
    int i;
    for(i=0;i<v->n;i++)
      {
	  printf("%d %d\n",i,((long int *)v->a)[i]);
      }
}

int main(int argc,char *argv[])
{
    varray *v;
    long int *i;
    v=varray_init(16,sizeof(long int),-1);
    i = varray_ptr(v,10);
    *i=12345L;
    varray_print(v);
    i = varray_ptr(v,100);
    *i=23456L;
    varray_print(v);
    varray_done(v);
}
*/
