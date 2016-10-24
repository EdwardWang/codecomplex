#ifndef _ITERATOR_H_
#define _ITERATOR_H_

typedef struct iterator_s iterator_t;

iterator_t *
iterator_init(void *collection);

int 
iterator_step(iterator_t *iterator, void *element);


void 
iterator_free(iterator_t *iterator);

#endif
