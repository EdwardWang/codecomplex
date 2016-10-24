#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmplx_file_manage.h"
#include "cmplx_core.h"

/*complex_string*/

char *complex_string(const char *str)
{
    int i;
    char *s = strdup(str);
    for (i = 0; i < strlen(s); i++) {
        s[i] = '_';
    }
    return s;
}

int main(int argc, char *argv[])
{
    cmplx_core_t *core;
    cmplx_file_manage_t *file_manage = NULL;
    file_manage = cpmlx_file_init("","");
    core = cmplx_core_init("C");
    if (file_manage && core) {
        char *filename = "D:/main.c";
        while ( (filename = cmplx_file_get_a_file(file_manage)) != NULL) {
           cmplx_core_parse_table(core,filename); 
           free(filename);
        }
        cmplx_core_complex_code(core, complex_string);
    }

    cmplx_core_free(core);
    cmplx_file_manage_free(file_manage);
	return 0;
}
