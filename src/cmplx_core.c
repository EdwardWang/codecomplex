#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Lib/list.h"
#include "../lib/btree.h"
#include "../lib/iterator.h"
#include "cmplx_core.h"
#include "cmplx_module.h"

struct cmplx_core_s {
    cmplx_module_t *module;
    tree_t *t;
};
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

struct token_s{
	char *token;
    char *complex_token;
	list_t *position;
};

static void create_token(void *p, void *key, void *data)
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

static int cmp_token(void *k1, void *k2)
{
	char *p1 = (char *)k1;
	struct token_s *p2 = (struct token_s *)k2;
	return strcmp(p1,p2->token);
}

static void print_token(void *p)
{
	struct token_s *tp = (struct token_s *)p;
	printf("token:%s\n",tp->token);
	list_print(tp->position,print_udata);
}

static void free_token(void *p)
{
    struct token_s *t = (struct token_s *)p;
    free(t->token);
    list_free(t->position);
}

cmplx_core_t *cmplx_core_init(const char *module_name)
{
    struct cmplx_core_s *core = NULL;

    core = (struct cmplx_core_s *)malloc(sizeof(struct cmplx_core_s));
    if (core) {
        core->module = cmplx_module_get_by_name(module_name);
        core->module->init_pf();
        core->t = tree_init(sizeof(struct token_s), create_token, cmp_token);
    }
    return core;
}

int cmplx_core_parse_table(cmplx_core_t *core, const char *file)
{
    FILE *fp = NULL;
    struct udata data;
    cmplx_module_token_t token;

    if ((fp = fopen(file, "r")) != NULL) {
        data.filename = strdup(file);
        while (core->module->scan_token_pf(fp,&token) != EOF) {
            data.offset = token.offset;
            tree_insert(core->t,token.token,&data);
        }
        fclose(fp);
		//tree_print(core->t, print_token);
        return 0;
    }
    fclose(fp);
    return -1; 
}

//这里用迭代器实现
int cmplx_core_complex_code(cmplx_core_t *core, cmplx_complex_algorithm_pf complex_fun)
{
    char *cstr = NULL;
    char *filename = NULL;
	struct udata *data;
	struct token_s tree_data;
	cmplx_module_token_t mtoken_s;

    iterator_t *iterator = iterator_init(core->t);

    while (iterator_step(iterator, &tree_data) != 1) {
        cstr = complex_fun(tree_data.token);
        while ((data = list_pop(tree_data.position)) != NULL) {
			mtoken_s.token = cstr;
			mtoken_s.offset = data->offset;
			core->module->amend_token_pf(data->filename, &mtoken_s,strlen(tree_data.token));
		}
		free(cstr);
		cstr = NULL;
    }
    free(iterator);

	return 0;
}

void cmplx_core_free(cmplx_core_t *core)
{
    tree_free(core->t, free_token);
    free(core);
    core->module->exit_pf();
}
