#include <stdlib.h>
#include "list.h"

struct lnode_s {
	void *data;
	struct lnode_s *next;
};

struct list_s {
	cmplx_create_udata_pf create_udata_pf;
	cmplx_free_udata_pf free_udata_pf;
	compare compare_udata_pf;
	unsigned int count;
	struct lnode_s *head;
};

list_t *list_init(cmplx_create_udata_pf create_udata_pf, 
				  cmplx_free_udata_pf free_data_pf,
				  compare compare_pf)
{
	struct list_s *l = NULL;

	l = (struct list_s*)malloc(sizeof(struct list_s));
	if (l != NULL) {
		l->create_udata_pf = create_udata_pf;
		l->free_udata_pf = free_data_pf;
		l->compare_udata_pf = compare_pf;
		l->count = 0;
		l->head = NULL;
	}
	return l;
}

void list_free(list_t *l)
{
	struct lnode_s *p = NULL;

	for (p = l->head; p != NULL; p = l->head) {
		l->free_udata_pf(p->data);
		l->head = p->next;
		free(p);
	}
	free(l);
}

static struct lnode_s *list_make_node(cmplx_create_udata_pf create_pf, void *data)
{
	struct lnode_s *node = NULL;
	node = (struct lnode_s *)malloc(sizeof(struct lnode_s));
	if (node) {
		node->data = create_pf(data);
		node->next = NULL;
	}
	return node;
}

static int is_in_list(list_t *l, void *data)
{
	struct lnode_s *p = NULL;
	for (p = l->head; p != NULL;p = p->next) {
		if (!l->compare_udata_pf(p->data, data))
		{
			return 1;
		}
	}
	return 0;
}

int list_insert(list_t *l, void *data)
{
	struct lnode_s *tmp = NULL;
	if (l->head == NULL) {
		tmp = list_make_node(l->create_udata_pf, data);
		if (tmp != NULL) {
			l->count++;
			l->head = tmp;
		}
	} else {		/* 头不为空，则插入 */
		if (!is_in_list(l,data)) {
			tmp = list_make_node(l->create_udata_pf, data);
			if(tmp != NULL) {
				tmp->next = l->head->next;
				l->head->next = tmp;
				l->count++;
			}
		}
	}
	return 0;
}

int list_delete(list_t *l, void *data)
{
	struct lnode_s *p, *fore;

	for(p = l->head, fore = l->head; p != NULL; p = p->next) {
		if (!l->compare_udata_pf(data,p->data)) {
			fore->next = p->next;
			p->next = NULL;
			l->free_udata_pf(p->data);
			p->data = NULL;
			free(p);
			l->count--;
			return 0;
		}
		fore = p;
	}
	return 1;
}


unsigned int list_count(list_t *l)
{
	return l->count;
}

void list_print(list_t*l, cmplx_print_udata_pf print_udata)
{
	struct lnode_s *p;

	for (p = l->head; p != NULL; p = p->next) {
		print_udata(p->data);
	}
}

void *list_pop(list_t *l)
{ 
    void *data = NULL;
    struct lnode_s *node = l->head;
    if (l->head != NULL) {
        l->count -= 1;
        l->head = l->head->next;
        data = node->data;
        free(node);
    }
    return data;
}
