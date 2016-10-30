#ifndef _CMPLX_MODULE_INTERFACE_H_
#define _CMPLX_MODULE_INTERFACE_H_

#include <stdio.h>

typedef struct{
	char *token;
	unsigned int offset;
}cmplx_module_token_t;

/*
 * module��ʼ��
 * 0��ʾ��ʼ���ɹ�,������ʾʧ��
 * */
typedef int (*cmplx_module_init_pf)(void);

/*
 * module�˳�����
 * 0��ʾ�ɹ���������ʾʧ��
 */
typedef int (*cmplx_module_exit_pf)(void);

/*
 * C���Խ�����ģ����Ҫ����token�����ƺ�λ��
 * cmplx_module_token_t �ṹ�������ģ���������һ��ʹ��
 */
typedef int (*cmplx_module_scan_token_pf)(FILE *fp, cmplx_module_token_t *token);

/*
 * ���Ҵ����token�����ػ��ҵ�token�����ɵ�tokenӦ����������Ե��﷨����
 * ������NULL
 */
typedef char *(*cmplx_module_complex_token_pf)(const char *token);

/*
 * �ж��ļ��Ƿ���Ҫ����,���ڹ��˷Ǳ�ģ����ļ�
 * */
typedef int (*cmplx_module_filter_pf)(const char *filename);

#endif
