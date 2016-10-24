#ifndef _BTREE_H_
#define _BTREE_H_

#include "common.h"
typedef struct tree_s tree_t;

typedef void (*cmplx_btree_init_udata_pf)(void *np, void *key,void *data);
typedef void (*cmplx_btree_free_udata_pf)(void *data);
typedef void (*cmplx_btree_print_udata_pf)(void *data);
typedef void (*cmplx_btree_node_func)(void *np, void *client_data);

tree_t *
tree_init(unsigned int udatasize, 
		  cmplx_btree_init_udata_pf init_pf, 
		  compare cmp_pf);

void 
tree_free(tree_t *t, cmplx_btree_free_udata_pf free_pf);

int 
tree_insert(tree_t *t, void *key, void *data);

void *
tree_get_adata(tree_t *t, void *key);

void 
tree_print(tree_t *t, cmplx_btree_print_udata_pf print_pf);

void 
tree_map(tree_t *t, cmplx_btree_node_func func, void *client_data);

#endif
