#include "../lib/list.h"
#include "../lib/btree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct udata{
	char *filename;
	unsigned int offset;
};

void *make_udata(void *udata)
{
	struct udata *tmp;
	struct udata *ud = (struct udata *)udata;
	tmp = (struct udata *)malloc(sizeof(struct udata));
	if (tmp != NULL) {
		tmp->filename = strdup(ud->filename);
		tmp->offset = ud->offset;
	}
	return tmp;
}

void free_udata(void *udata)
{
	struct udata *ud = (struct udata *)udata;
	free(ud->filename);
	free(ud);
}

int cmp_udata(void *a, void *b)
{
	int r;
	struct udata *ca = (struct udata *)a;
	struct udata *cb = (struct udata *)b;
	if(!(r = strcmp(ca->filename,cb->filename))) {
		return (ca->offset - cb->offset);
	}
	return r;
}

void print_udata(void *data)
{
	struct udata *d = data;
	printf("filename = %s, offset = %d\n", d->filename,d->offset);
}

void test_list()
{
	list_t *l;
	struct udata ud;
	l = list_init(make_udata, free_udata, cmp_udata);
	if (l == NULL) {
		fprintf(stderr,"list init failed!\n");
		return;
	}
	
	ud.filename = strdup("test.c");
	ud.offset = 15;
	list_insert(l,&ud);
	free(ud.filename);
	ud.filename = strdup("MainCtrl/test.c");
	ud.offset = 485;
	list_insert(l,&ud);
	list_print(l,print_udata);
	list_free(l);
}

struct token_s{
	char *token;
	list_t *position;
};

void create_token(void *p, void *key, void *data)
{
	struct token_s *to = (struct token_s *)p;

	if (to->token == NULL) {
		to->token = strdup((char *)key);
	}

	if (to->position == NULL) {
		to->position = list_init(make_udata, free_udata, cmp_udata);
	}
	list_insert(to->position, data);
}

int cmp_token(void *k1, void *k2)
{
	char *p1 = (char *)k1;
	struct token_s *p2 = (struct token_s *)k2;
	return strcmp(p1,p2->token);
}

void print_token(void *p)
{
	struct token_s *tp = (struct token_s *)p;
	printf("token:%s\n",tp->token);
	list_print(tp->position,print_udata);
}

void test_tree()
{
	tree_t *t;struct udata ud;
	t = tree_init(sizeof(struct token_s),create_token,cmp_token);

	ud.filename = strdup("test.c");
	ud.offset = 15;
	tree_insert(t,"udata", &ud);
	free(ud.filename);
	ud.filename = strdup("MainCtrl/test.c");
	ud.offset = 485;
	tree_insert(t,"tree", &ud);

	ud.offset = 500;
	tree_insert(t,"tree", &ud);

	free(ud.filename);
	ud.filename = strdup("main.c");
	ud.offset = 960;
	tree_insert(t,"cloud", &ud);
	ud.offset = 961;
	tree_insert(t,"cloud",&ud);

	tree_print(t,print_token);
}