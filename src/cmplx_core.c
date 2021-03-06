#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Lib/list.h"
#include "../lib/btree.h"
#include "../lib/iterator.h"
#include "cmplx_core.h"
#include "cmplx_module.h"

struct cmplx_core_s {
    cmplx_module_t *module;
    tree_t *t;
};
struct udata{
	char *filename;
	unsigned int offset;
};

void *make_udata(void *udata)
{
	struct udata *tmp;
	struct udata *ud = (struct udata *)udata;
	tmp = (struct udata *)malloc(sizeof(struct udata));
	if (tmp != NULL) {
		tmp->filename = strdup(ud->filename);
		tmp->offset = ud->offset;
	}
	return tmp;
}

void free_udata(void *udata)
{
	struct udata *ud = (struct udata *)udata;
	free(ud->filename);
	free(ud);
}

int cmp_udata(void *a, void *b)
{
	int r;
	struct udata *ca = (struct udata *)a;
	struct udata *cb = (struct udata *)b;
	if(!(r = strcmp(ca->filename,cb->filename))) {
		return (ca->offset - cb->offset);
	}
	return r;
}

void print_udata(void *data)
{
	struct udata *d = data;
	printf("filename = %s, offset = %d\n", d->filename,d->offset);
}

struct token_s{
	char *token;
    char *complex_token;
	list_t *position;
};

static void create_token(void *p, void *key, void *data)
{
	struct token_s *to = (struct token_s *)p;

	if (to->token == NULL) {
		to->token = strdup((char *)key);
        to->complex_token = NULL;
	}

    if (to->position == NULL) {
		to->position = list_init(make_udata, free_udata, cmp_udata);
	}
	list_insert(to->position, data);
}

static int cmp_token(void *k1, void *k2)
{
	char *p1 = (char *)k1;
	struct token_s *p2 = (struct token_s *)k2;
	return strcmp(p1,p2->token);
}

static void print_token(void *p)
{
	struct token_s *tp = (struct token_s *)p;
	printf("token:%s\n",tp->token);
	list_print(tp->position,print_udata);
}

static void free_token(void *p)
{
    struct token_s *t = (struct token_s *)p;
    free(t->token);
    free(t->complex_token);
    list_free(t->position);
}

cmplx_core_t *cmplx_core_init(const char *module_name)
{
    struct cmplx_core_s *core = NULL;

    core = (struct cmplx_core_s *)malloc(sizeof(struct cmplx_core_s));
    if (core) {
        core->module = cmplx_module_get_by_name(module_name);
        core->module->init();
        core->t = tree_init(sizeof(struct token_s), create_token, cmp_token);
    }
    return core;
}

int cmplx_core_parse_table(cmplx_core_t *core, const char *file)
{
    FILE *fp = NULL;
    struct udata data;
    cmplx_module_token_t token;

    if (!core->module->is_module_file(file))
        return 0;
    if ((fp = fopen(file, "r")) != NULL) {
        data.filename = strdup(file);
        while (core->module->scan_token(fp,&token) != EOF) {
            data.offset = token.offset;
            tree_insert(core->t,token.token,&data);
        }
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return -1; 
}

int cmplx_core_complex_code(cmplx_core_t *core, char *filename)
{
    FILE *fp = NULL;
    FILE *tmp_fp = NULL;
    int curpos,prevpos;
    int i, c;
    cmplx_module_token_t token;
    struct token_s *data = NULL;
	int outlen;

    if (!core->module->is_module_file(filename))
        return 0;

    tmp_fp = fopen("tmp", "w+");
    curpos = 0;
    if ((fp = fopen(filename, "r")) != NULL) {
		prevpos = ftell(fp);
        while (core->module->scan_token(fp,&token) != EOF) {
            curpos = token.offset;
			fseek(fp,prevpos,SEEK_SET);
			outlen = curpos-strlen(token.token);
            for(i = prevpos; i < outlen; i++) {
				c = fgetc(fp);
#ifdef _WIN32
				if (c == 0x0a) {	//读文件遇到换行时，相当于两个字符，
					i++;			//指针会向后移动，多移动了一次,但客户外部计数器并不知道，
					fputc('\n',tmp_fp);//导致文件指针向后偏移
					continue;
				}
#endif
                fputc(c, tmp_fp);
            }
            tree_get_adata(core->t, token.token, &data);

            if (data != NULL) {
                if (data->complex_token == NULL) {
                    data->complex_token = core->module->complex_token(token.token);
                }
                if (data->complex_token != NULL) {
				    fputs(data->complex_token, tmp_fp);
                }
            }
           fseek(fp,curpos+1, SEEK_SET); 
		   prevpos = curpos;
        }
        fseek(fp,curpos,SEEK_SET);
        while ((i = fgetc(fp)) != EOF) {
            fputc(i, tmp_fp);
        }
        fclose(fp);
    }
    
    fp = fopen(filename, "w");
    fseek(tmp_fp, 0, SEEK_SET);
    while ((i = fgetc(tmp_fp)) != EOF) {
        fputc(i, fp);
    }
    fclose(fp);
    fclose(tmp_fp);
    remove("tmp");
    return 0;
}

void cmplx_core_free(cmplx_core_t *core)
{
    tree_free(core->t, free_token);
    core->module->exit();
    free(core);
}
