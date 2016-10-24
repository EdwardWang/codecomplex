#include <stdlib.h>
#include <string.h>
#include "iterator.h"
#include "itertype.h"

#define ITERATOR_PASSWORD   3141582653UL

typedef struct cell_s{
    struct cell_s *link;
} cell_t;

struct iterator_s {
    int elementsize;
    compare cmp;
    cell_t *head, *tail;
};

iterator_t *iterator_init(void *collection)
{
    iterator_header_t *hp = collection;
    if (hp->password != ITERATOR_PASSWORD) {
        return NULL;
    }
    return hp->create_iterator(collection);
}

int iterator_step(iterator_t *iterator, void *element)
{
    cell_t *cp;
    void *dp;
    cp = iterator->head;
    if (cp == NULL) {
        iterator->tail = NULL;
        return 1;
    }
    dp = ((char *)cp) + sizeof(cell_t);
    memcpy(element, dp, iterator->elementsize);
    iterator->head = cp->link;
    free(cp);
    return 0;
}

void iterator_free(iterator_t *iterator)
{
    cell_t *cp;

    while ((cp = iterator->head) != NULL) {
        iterator->head = cp->link;
        free(cp);
    }
    free(iterator);
}

void iterator_enable(void *collection, new_iterator iterfn)
{
    iterator_header_t *hp = collection;

    hp->password = ITERATOR_PASSWORD;
    hp->create_iterator = iterfn;
}

iterator_t *iterator_init_list(int size, compare cmp)
{
    iterator_t *iterator;

    iterator = (iterator_t *)malloc(sizeof(iterator_t));

    iterator->elementsize = size;
    iterator->cmp = cmp;
    iterator->head = iterator->tail = NULL;
    return (iterator);
}

void iterator_add_item(iterator_t *iterator, void *ep)
{
    cell_t *np, *pp, *ip;
    void *dp;

    np = (cell_t *)malloc(sizeof(cell_t)+iterator->elementsize);
    dp = ((char *)np + sizeof(cell_t));
    memcpy(dp,ep,iterator->elementsize);
    pp = NULL;
    if (iterator->tail != NULL) {
        if (iterator->cmp == unsorted) {
            pp = iterator->tail;
        } else {
            dp = ((char *) iterator->tail + sizeof(cell_t));
            if (iterator->cmp(ep, dp) >= 0) pp = iterator->tail;
        }
    }
    if (pp == NULL) {
        for (ip = iterator->head; ip != NULL; ip = ip->link) {
            dp = ((char *) ip) + sizeof(cell_t);
            if (iterator->cmp(ep,dp) < 0) break;
            pp = ip;
        }
    }
    if (pp == NULL) {
        np->link = iterator->head;
        if (iterator->head == NULL) iterator->tail = np;
        iterator->head = np;
    } else {
        np->link = pp->link;
        if (pp->link == NULL) iterator->tail = np;
        pp->link = np;
    }
}
