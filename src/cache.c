/* キャッシュの実装
 * - 固定サイズのキャッシュ配列を管理
 * - キャッシュに存在しない要素へのアクセス時は、最も古い要素を削除して新しい要素のための場所を確保
 * - 現在は先読み機能は実装していない（先読みを実装する場合は、destructorに加えて
 *   データ追加用の関数が必要で、読み込み戦略の検討が必要）
 * - 大規模なキャッシュの場合は、現在の前方からの線形探索では時間がかかりすぎるため、
 *   ハッシュテーブルとの組み合わせを検討する必要がある
 */

/*非常に大きなキャッシュを作る場合には、このプログラムのように前方から */
/*順番に探索する方法では時間がかかりすぎる。hashと組みあわせるようなこ */
/*とも必要かも。*/
#include "cache.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* キャッシュの破棄
 * - キャッシュ内の全要素を解放
 * - キャッシュ自体のメモリーも解放
 */
void Cache_Destroy(Cache *c)
{
	if (c == NULL) return;
	
	// 残っている要素を解放
	for (int i = 0; i < c->n; i++) {
		if (c->ptr[i] != NULL) {
			c->destructor(c->ptr[i]);
		}
	}
	
	free(c->index);
	free(c->ptr);
	free(c);
}

/* キャッシュの初期化
 * - 指定されたサイズのキャッシュを作成
 * - 要素の解放用のdestructor関数を設定
 * - メモリー確保失敗時はNULLを返す
 */
Cache *Cache_Init(int n, void (*destructor)(void *))
{
	if (n <= 0 || destructor == NULL) return NULL;
	
	Cache *c = malloc(sizeof(Cache));
	if (c == NULL) return NULL;
	
	c->destructor = destructor;
	c->n = n;
	
	c->index = malloc(sizeof(int) * n);
	c->ptr = malloc(sizeof(void *) * n);
	
	if (c->index == NULL || c->ptr == NULL) {
		if (c->index) free(c->index);
		if (c->ptr) free(c->ptr);
		free(c);
		return NULL;
	}
	
	memset(c->index, -1, sizeof(int) * n);
	memset(c->ptr, (intptr_t)NULL, sizeof(void *) * n);
	return c;
}

/* キャッシュ内の要素へのポインタを取得
 * - 要素が見つかった場合は、その要素を最新の位置に移動
 * - 要素が見つからない場合は、最も古い要素を削除して新しい要素のための場所を確保
 */
void **Cache_ptr(Cache *c, int i)
{
	if (c == NULL) return NULL;
	
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
	
	// キャッシュが空の場合の処理を修正
	if (c->n > 0) {
		j = c->n - 1;
		if (c->ptr[j] != NULL) {
			c->destructor(c->ptr[j]);
		}
		
		for (k = j; k > 0; k--) {
			c->index[k] = c->index[k - 1];
			c->ptr[k] = c->ptr[k - 1];
		}
		c->index[0] = i;
		c->ptr[0] = NULL;
	}
	return c->ptr;
}

/* キャッシュの内容を表示（デバッグ用） */
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
