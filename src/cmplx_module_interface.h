#ifndef _CMPLX_MODULE_INTERFACE_H_
#define _CMPLX_MODULE_INTERFACE_H_

#include <stdio.h>

typedef struct{
	char *token;
	unsigned int offset;
}cmplx_module_token_t;

/*
 * C���Խ�����ģ����Ҫ����token�����ƺ�λ��
 * cmplx_module_token_t �ṹ�������ģ���������һ��ʹ��
 */
typedef int (*cmplx_module_scan_token_pf)(FILE *fp, cmplx_module_token_t *token);

/*
 * �޸�fp�е�token
 * len Ϊԭtoken�ĳ���
 */
typedef int (*cmplx_module_amend_token_pf)(char *filename, cmplx_module_token_t *token, int len);

#endif