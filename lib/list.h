#ifndef _LIST_H_
#define _LIST_H_

#include "common.h"
typedef struct list_s list_t;

typedef void* (*cmplx_create_udata_pf)(void *);
typedef void (*cmplx_free_udata_pf)(void *);
typedef void (*cmplx_print_udata_pf)(void *);

list_t *
list_init(cmplx_create_udata_pf create_udata_pf,
		  cmplx_free_udata_pf free_data_pf, compare compare_pf);

void 
list_free(list_t *l);

int 
list_insert(list_t *l, void *data);

int 
list_delete(list_t *l, void *data);

unsigned int 
list_count(list_t *l);

void *
list_pop(list_t *l);

void 
list_print(list_t*l, cmplx_print_udata_pf print_udata);
#endif
