#ifndef _CMPLX_MODULE_C_H_
#define _CMPLX_MODULE_C_H_
#include "../src/cmplx_module_interface.h"

#define CMPLX_MC_NAME	"C"

int cmplx_mc_scan_token(FILE *fp, cmplx_module_token_t *token);
int cmplx_mc_amend_token(char *filename, cmplx_module_token_t *token,int len);

#endif