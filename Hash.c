/*標準的なオープンアドレスハッシュ。与えられた要素とハッシュ内にある要
  素の比較を厳重に行えるように、外部関数を呼びだすように設計する。*/
#include <stdio.h>
#include <stdlib.h>
#include "Hash.h"

/*hash要素の番号を返す。もし存在しない要素なら、空き要素を返すが、デー
  タの挿入はしない。*/
int Hash_QueryElement(sHash *h,unsigned int key,void *element)
{
  int dup=0;
  int d=13;
  while(1){
    if(h->value[key]==NULL){
      /*fprintf(stderr,"(emp.%d)",key);*/
      return key;
    }else{
      if(h->_hash_compare(h->value[key],element)==0){
	/*fprintf(stderr,"(ful.%d)",key);*/
	return key;
      }
    }
    key+=d;
    d++;
    dup++;
    /*fprintf(stderr,"(%d:%d)",dup,key);*/
    if(key>=h->hashsize)
      key-=h->hashsize;
  }
}

/*値を追加登録する。すでにスロットが占有されていれば1を返す。*/
int Hash_RegisterValue(sHash *h,unsigned int key,void *element)
{
  if(h->value[key]==NULL){
    h->value[key]=element;
    h->nentry++;
    if(h->nentry > h->hashsize/2){
      fprintf(stderr,"Warning: hash size seems too small.\n");
    }
    return 0;
  }else{
    return 1;
  }
}

sHash *Hash_Init(int hashsize,int (*compar)(const void *,const void *))
{
  sHash *h=malloc(sizeof(sHash));
  h->hashsize=hashsize;
  h->value=(void **)calloc(hashsize,sizeof(void *));
  h->nentry=0;
  h->_hash_compare=compar;
  return h;
}

void Hash_Done(sHash *h)
{
  free(h->value);
  free(h);
}
