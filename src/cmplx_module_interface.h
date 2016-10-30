#ifndef _CMPLX_MODULE_INTERFACE_H_
#define _CMPLX_MODULE_INTERFACE_H_

#include <stdio.h>

typedef struct{
	char *token;
	unsigned int offset;
}cmplx_module_token_t;

/*
 * module初始化
 * 0表示初始化成功,其他表示失败
 * */
typedef int (*cmplx_module_init_pf)(void);

/*
 * module退出函数
 * 0表示成功，其他表示失败
 */
typedef int (*cmplx_module_exit_pf)(void);

/*
 * C语言解析的模块需要返回token的名称和位置
 * cmplx_module_token_t 结构体给解析模块和主程序一起使用
 */
typedef int (*cmplx_module_scan_token_pf)(FILE *fp, cmplx_module_token_t *token);

/*
 * 混乱传入的token，返回混乱的token，生成的token应当满足该语言的语法规则
 * 出错返回NULL
 */
typedef char *(*cmplx_module_complex_token_pf)(const char *token);

/*
 * 判断文件是否需要解析,用于过滤非本模块的文件
 * */
typedef int (*cmplx_module_filter_pf)(const char *filename);

#endif
