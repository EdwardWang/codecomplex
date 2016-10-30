#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmplx_file_manage.h"
#include "cmplx_core.h"

int main(int argc, char *argv[])
{
    char *filename = NULL;
    cmplx_core_t *core;
    cmplx_file_manage_t *file_manage = NULL;
    file_manage = cpmlx_file_init("tmpdir1","tmpdir2");
    core = cmplx_core_init("C");
    if (file_manage && core) {
        while ( (filename = cmplx_file_get_a_file(file_manage)) != NULL) {
            cmplx_core_parse_table(core,filename); 
            cmplx_core_complex_code(core, filename);
            free(filename);
        }
    }

    cmplx_core_free(core);
    cmplx_file_manage_free(file_manage);
	return 0;
}
