#ifndef _VARRAY_H
#define _VARRAY_H
/*可変長配列*/
#include <stdlib.h>
typedef struct
{
    int initval;
    size_t size;
    int n;
    void *a;
} varray;

varray * varray_init(size_t nmemb,size_t size,int initval);
void *varray_ptr(varray *v,int i);


#endif
