#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../lib/list.h"
#include "cmplx_file_manage.h"

struct cmplx_file_manage_s{
	char *src;
	char *dst;
	list_t *file;
	list_t *dir;
};

static void *get_name(void *name)
{
    return strdup((const char *)name);
}

static void free_name(void *name)
{
    free((char *)name);
}

static int cmp_name(void *n1, void *n2)
{
    return strcmp((const char *)n1, (const char *)n2);
}

static void get_files_and_dirs(char *src, list_t *file, list_t *dir)
{
    struct stat statbuf;
    if (lstat(src, &statbuf) < 0) return;
    if (S_ISREG(statbuf.st_mode)) {
        list_insert(file,src);
        return;
    }
    if (S_ISDIR(statbuf.st_mode)) {
        DIR *dp = NULL;
        struct dirent *dirp = NULL;
        char buf[128];
        if ((dp = opendir(src)) == NULL) {
            /* log: open dir %s failed! */
            printf("Open dir failed!");
            return;
        }
        memset(buf,0,128);
        while ((dirp = readdir(dp)) != NULL) {
            sprintf(buf, "%s/%s", src,dirp->d_name);
            //printf("%s\n", buf);//目录还是要使用绝对路径
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
                continue;
            }
            memset(&statbuf, 0, sizeof(struct stat));
            if (lstat(buf, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    list_insert(file,buf);
                }
                if (S_ISDIR(statbuf.st_mode)) {
                    list_insert(dir, buf);
                }
            } else {
                printf("dirname:%s\n", dirp->d_name);
            }
        }
    }
}

/*
 * src原源文件目录
 * dst存放目录
 */
cmplx_file_manage_t* cpmlx_file_init(const char *src, const char *dst)
{
    struct cmplx_file_manage_s *manage = NULL;

    manage  = (struct cmplx_file_manage_s *) malloc(sizeof(struct cmplx_file_manage_s));
    if (manage){
        char buf[1024];
        manage->src = strdup(src);
        manage->dst = strdup(dst);
        manage->file = list_init(get_name, free_name, cmp_name);
        manage->dir = list_init(get_name, free_name, cmp_name);
        sprintf(buf, "cp -rf %s %s", src, dst);
        system(buf);
        /*  检索一次目录，获取一些文件和目录 */
        get_files_and_dirs(manage->dst,manage->file, manage->dir);
    }
    return manage;
}

char *cmplx_file_get_a_file(cmplx_file_manage_t *manage)
{
    if (list_count(manage->file) == 0) {
        if (list_count(manage->dir) == 0) {
            /* log: no files more */
            printf("No more files\n");
            return NULL;
        } else {
            /* get a dir, get other files */
            char *dirname = list_pop(manage->dir);
            get_files_and_dirs(dirname, manage->file, manage->dir); 
        }
    }
    return list_pop(manage->file);
}

void cmplx_file_manage_free(cmplx_file_manage_t *manage)
{
    free(manage->src);
    free(manage->dst);
    list_free(manage->file);
    list_free(manage->dir);
    free(manage);
}
