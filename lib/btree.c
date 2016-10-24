#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "btree.h"
#include "itertype.h"

typedef void *itree_t;

struct tnode_s{
	itree_t left, right;
};

struct tree_s {
    iterator_header_t iterheader;
	unsigned int udatasize;
	unsigned int totalsize;
	cmplx_btree_init_udata_pf init_udata_pf;
	compare udata_cmp_pf;
	itree_t root;
};

static struct tnode_s *tree_get_tnode(tree_t *t,itree_t p)
{
	return (struct tnode_s *)((char *)p + t->udatasize);
}

void add_item_to_iterator(void *np, void *client_data)
{
   iterator_add_item((iterator_t *)client_data, np); 
}
static iterator_t *create_iterator(void *collection)
{
    tree_t *tree = collection;
	iterator_t *iterator;

    iterator = iterator_init_list(tree->udatasize, unsorted);
    tree_map(tree, add_item_to_iterator, iterator);

	return iterator;
}
tree_t *tree_init(unsigned int udatasize, cmplx_btree_init_udata_pf init_pf, compare cmp_pf)
{
	struct tree_s *t = NULL;

	t = (struct tree_s *)malloc(sizeof(struct tree_s));
	if (t){
		t->root = NULL;
        iterator_enable(t,create_iterator);
		t->totalsize = udatasize + sizeof(struct tnode_s);
		t->udatasize = udatasize;
		t->init_udata_pf = init_pf;
		t->udata_cmp_pf = cmp_pf;
	}
	return t;
}

static void free_tree_node(tree_t *t, itree_t *p, cmplx_btree_free_udata_pf free_pf)
{
	struct tnode_s *tn;
	tn = tree_get_tnode(t,*p);
	if (tn->left) free_tree_node(t, &tn->left, free_pf);
	if (tn->right) free_tree_node(t, &tn->right, free_pf);

	free_pf(*p);
	free(*p);
	*p = NULL;
}

void tree_free(tree_t *t, cmplx_btree_free_udata_pf free_pf)
{
	free_tree_node(t,&t->root,free_pf);
	free(t);
}

static int insert_data(tree_t *t, itree_t *p, void *key, void *data)
{
	int r;struct tnode_s *tn;
	if (*p == NULL) {
		*p = (itree_t)malloc(t->totalsize);
		if (*p) {
			memset(*p,0,t->totalsize);
			tn = tree_get_tnode(t,*p);
			t->init_udata_pf(*p,key,data);
			tn->left = tn->right = NULL;
			return 1;
		}
	}

	r = t->udata_cmp_pf(key, *p);
	tn = tree_get_tnode(t,*p);
	if (r > 0) {
		insert_data(t,&tn->right,key,data);
	} else if (r < 0) {
		insert_data(t,&tn->left,key,data);
	} else {
		t->init_udata_pf(*p,key,data);
	}
	return 0;
}

int tree_insert(tree_t *t, void *key, void *data)
{
	return insert_data(t, &t->root, key,data);
}

static void *get_data(tree_t *t, itree_t *p, void *key)
{
    char *s = (char *)key;
    struct tnode_s *tn = NULL;
    tn = tree_get_tnode(t, *p);
    if (key == NULL) {
        if (tn->left) get_data(t, &tn->left,key);
        if (tn->right) get_data(t, &tn->right,key);
        return *p;
    } else {
		int cmp = 1;
		cmp = t->udata_cmp_pf(key, *p);
        if (cmp == 0) { return *p;}

		if (!tn->left && !tn->right)
		{
			return NULL;
		}

		if (cmp < 0){
			if (tn->left) get_data(t, &tn->left, key);
		}
		if (cmp > 0) {
			if (tn->right) get_data(t, &tn->right, key);
		}
    }
}

void *tree_get_adata(tree_t *t, void *key)
{
    return get_data(t, &t->root, key);   
}

static void print_tree(tree_t *t, itree_t p,cmplx_btree_print_udata_pf print_pf)
{
	struct tnode_s *tn;
	if (p == NULL) return;
	tn = tree_get_tnode(t,p);
	print_tree(t,tn->left,print_pf);
	print_pf(p);
	print_tree(t,tn->right,print_pf);
}

/* mid order */
void tree_print(tree_t *t, cmplx_btree_print_udata_pf print_pf)
{
	print_tree(t, t->root, print_pf);
}


static void map(tree_t *t, itree_t p, cmplx_btree_node_func func, void *client_data)
{
	struct tnode_s *tn;
	if (p == NULL) return;
	tn = tree_get_tnode(t,p);
	map(t,tn->left,func, client_data);
	func(p, client_data);
	map(t,tn->right,func, client_data);
}

void tree_map(tree_t *t, cmplx_btree_node_func func, void *client_data)
{
    map(t,t->root,func,client_data);
}