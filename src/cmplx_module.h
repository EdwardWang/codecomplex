#ifndef _CMPLX_MODULE_H_
#define _CMPLX_MODULE_H_

#include "cmplx_module_interface.h"

typedef struct {
	char *name;
	cmplx_module_scan_token_pf scan_token_pf;
	cmplx_module_amend_token_pf amend_token_pf;
}cmplx_module_t;

cmplx_module_t *cmplx_module_get_by_name(const char *name);

#endif
