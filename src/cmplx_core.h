#ifndef _CMPLX_CORE_H_
#define _CMPLX_CORE_H_
/* 本模块为流程 */

typedef struct cmplx_core_s cmplx_core_t;

typedef char *(*cmplx_complex_algorithm_pf)(const char *token);

cmplx_core_t *
cmplx_core_init(const char *module_name);

void
cmplx_core_free(cmplx_core_t *core);

int 
cmplx_core_parse_table(cmplx_core_t *core, const char *file);

int 
cmplx_core_complex_code(cmplx_core_t *core, cmplx_complex_algorithm_pf complex_fun);
#endif
