#ifndef _CMPLX_MODULE_C_H_
#define _CMPLX_MODULE_C_H_
#include "../src/cmplx_module_interface.h"

#define CMPLX_MC_NAME	"C"

int cmplx_mc_init(void);
int cmplx_mc_exit(void);
int cmplx_mc_scan_token(FILE *fp, cmplx_module_token_t *token);
char *cmplx_mc_complex_token(const char *token);

#endif
