#ifndef _CMPLX_MODULE_INTERFACE_H_
#define _CMPLX_MODULE_INTERFACE_H_

#include <stdio.h>

typedef struct{
	char *token;
	unsigned int offset;
}cmplx_module_token_t;

/*
 * C语言解析的模块需要返回token的名称和位置
 * cmplx_module_token_t 结构体给解析模块和主程序一起使用
 */
typedef int (*cmplx_module_scan_token_pf)(FILE *fp, cmplx_module_token_t *token);

/*
 * 修改fp中的token
 * len 为原token的长度
 */
typedef int (*cmplx_module_amend_token_pf)(char *filename, cmplx_module_token_t *token, int len);

#endif