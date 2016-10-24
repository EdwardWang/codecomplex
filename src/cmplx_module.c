#include "cmplx_module.h"
#include "../module/cmplx_module_c.h"
#include <string.h>

static cmplx_module_t cmplx_module_set[] = {
	{CMPLX_MC_NAME, cmplx_mc_init, cmplx_mc_exit, \
        cmplx_mc_scan_token, cmplx_mc_amend_token},
	{NULL,NULL,NULL},
};

cmplx_module_t *cmplx_module_get_by_name(const char *name)
{
	int i;

	for (i = 0; cmplx_module_set[i].name != NULL; i++) {
		if (!strcmp(cmplx_module_set[i].name, name)) {
			return &cmplx_module_set[i];
		}
	}
	return NULL;
}
