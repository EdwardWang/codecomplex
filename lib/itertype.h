#ifndef _ITERTYPE_H_
#define _ITERTYPE_H_

#include "iterator.h"
#include "common.h"

typedef iterator_t *(*new_iterator)(void *collection);

#define unsorted ((compare)0)

typedef struct {
    unsigned long password;
    new_iterator create_iterator;
} iterator_header_t;

void iterator_enable(void *collection, new_iterator create_iterator);

iterator_t *iterator_init_list(int size, compare cmp);

void iterator_add_item(iterator_t *iterator, void *ep);

#endif
