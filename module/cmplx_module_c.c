#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include "cmplx_module_c.h"

typedef enum {
	mc_start_s,					/* 开始解析 */
	mc_precompile_s,			/* 预编译 */
	mc_precomment_s,			/* 需要拿到第2个字符才能确定是不是注释 */
	mc_line_comment_s,			/* 行注释模式 */
	mc_star_comments_s,			/* *注释模式 */
	mc_star_comments_end_s,		/* *注释结束标志 */
	mc_comments_end_s,			/* 注释结束标志 */
	mc_string_s,				/* 字符串 */
	mc_escape_string_s,			/* 转义字符串" */
	mc_string_end_s,			/* 字符串结束标志 */
	mc_char_s,					/* 字符 */
	mc_escape_char_s,			/* 转义字符 */
	mc_char_end_s,				/* 字符结束标志*/
	mc_function_s,				/* 函数 */
	mc_token_s,					/* token */
	mc_token_end_s,				/* 结束token解析 */
	mc_state_maxnum
}mc_state_s;

#define MC_MAXBUFSIZE	1024
#define MC_MAXTOKENLEN  30

static const char *keywords[] = {
	"void","double","int","float","char",
    "short","long","unsigned","signed",
	"struct","union","enum","typedef","sizeof",
	"auto","static","register","extern","const",
    "volatile","return","continue","break","goto",
	"if","else","switch","case","default","for",
	"do","while",NULL
};

static const char *pre_char = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *other_char = "0123456789\
                                 _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int cmplx_mc_init(void)
{
    srand((unsigned)time(NULL));
    return 0;
}

int cmplx_mc_exit(void)
{
    return 0;
}

static int range_rand(int i, int n)
{
    return (rand()%(n-i+1) + i);
}

/*
 * 功能：输入字符串是否是C语言关键字
 * 返回值:0--不是关键字；1--是关键字
 */
int iskeyword(const char *word)
{
	int i = 0;
	for (i = 0; keywords[i] != NULL; i++) {
		if (!strcmp(word,keywords[i])) {
			return 1;
		}
	}
	return 0;
}

static mc_state_s state(mc_state_s s, int c)
{
	
	switch(s) {
	case mc_start_s:
		if(c == '_' || isalpha(c)) {
			s = mc_token_s;
		}
		else if (c == '/') {
			s = mc_precomment_s;
		}
		else if (c == '#'){
			s = mc_precompile_s;
		}
		else if (c == '"') {
			s = mc_string_s;
		}
		else if (c == '\'') {
			s = mc_char_s;
		} else {
			s = mc_start_s;
		}
		break;
	case mc_precompile_s:
		if (c == '\n') {
			s = mc_start_s;
		} else {
			s = mc_precompile_s;
		}
		break;
	case mc_precomment_s:
		if(c == '/' ){
			s = mc_line_comment_s;
		} else if (c == '*'){
			s = mc_star_comments_s;
		} else {
			s = mc_start_s;
		}
		break;
	case mc_line_comment_s:
		if (c == '\n') {
			s = mc_comments_end_s;
		} else {
			s = mc_line_comment_s;
		}
		break;
    case mc_star_comments_s:
		if (c == '*') {
			s = mc_star_comments_end_s;
		}else {
			s = mc_star_comments_s;
		}
		break;
    case mc_star_comments_end_s:
		if (c == '/') {
			s = mc_comments_end_s;
		} else {
			s = mc_star_comments_s;
		}
		break;
	case mc_string_s:
		if (c == '\\') {
			s = mc_escape_string_s;
		} else if (c == '"') {
			s = mc_string_end_s;
		} else {
			s = mc_string_s;
		}
		break;
	case mc_escape_string_s:
		s = mc_string_s;break;
	case mc_char_s:
		if (c == '\\') {
			s = mc_escape_char_s;
		} else if (c == '\'') {
			s = mc_char_end_s;
		} else {
			s = mc_char_s;
		}
		break;
	case mc_escape_char_s:
		s = mc_char_s;break;
	case mc_token_s:
		if (!isalnum(c)) {
			if (c == '_') {
				s = mc_token_s;
			} else if (c == '(') {
				s = mc_function_s;
			} else {
				s = mc_token_end_s;
			}
		} else {
			s = mc_token_s;
		}
		break;
    case mc_comments_end_s:
	case mc_string_end_s:
	case mc_char_end_s:
	case mc_token_end_s:
	default:
		s = mc_start_s;break;
	}
	return s;
}

int 
cmplx_mc_scan_token(FILE *fp, cmplx_module_token_t *token)
{
	int c, i;
	static int count = 0;
	mc_state_s s;
	char buf[MC_MAXBUFSIZE];

	c = EOF;
	s = mc_start_s;
	i = 0;
	memset(buf,0,MC_MAXBUFSIZE);
	while ( (c = fgetc(fp)) != EOF) {
		count++;
		s = state(s,c);
		switch(s) {
		case mc_token_s:
			buf[i++] = c;
			break;
		case mc_token_end_s:
			buf[i] = '\0';
			if (!iskeyword(buf)) {
				token->token = strdup(buf);
				token->offset = count-strlen(buf);
				return 0;
			} else {
				c = EOF;
				s = mc_start_s;
				i = 0;
				continue;
			}
		case mc_function_s:
			c = EOF;
			s = mc_start_s;
			i = 0;
			continue;
		case mc_start_s:
		case mc_precompile_s:
		case mc_precomment_s:
		case mc_line_comment_s:
		case mc_star_comments_s:
		case mc_star_comments_end_s:
		case mc_comments_end_s:
		case mc_string_s:
		case mc_escape_string_s:
		case mc_string_end_s:
		case mc_char_s:
		case mc_escape_char_s:
		case mc_char_end_s:
		default:continue;
		}
	}
	count = 0;
	return c;
}

char * 
cmplx_mc_complex_token(const char *token)
{
    int i, pos, newlen;
    char buf[MC_MAXTOKENLEN+1];    
    memset(buf, 0, MC_MAXTOKENLEN+1);

    if (*token != '\0') {
        newlen = range_rand(1,MC_MAXTOKENLEN);
        pos = range_rand(0,strlen(pre_char)-1);
        buf[0] = pre_char[pos];
        for (i = 1; i < newlen;i++) {
            pos = range_rand(0,strlen(other_char)-1);
            buf[i] = other_char[pos];
        }
        buf[i] = '\0';
        return strdup(buf);
    } else {
        return NULL;
    }
    //选择初始字符
    /*
	int c;
	unsigned int count;

	FILE *fp, *tmpfp;

	fp = fopen(filename,"r");
	tmpfp = fopen("tmp","wt+");

	count = 0;
	while ((c = fgetc(fp)) != EOF) {
		count++;
		if (count == token->offset) {
			fputs(token->token,tmpfp);
			fseek(fp,len-1,SEEK_CUR);
			count += (len-1);
		} else {
			fputc(c,tmpfp);
		}
	}
	fclose(fp);
	fclose(tmpfp);

	fp = fopen(filename,"w");
	tmpfp = fopen("tmp","r");

	while ((c = fgetc(tmpfp)) != EOF) {
		fputc(c,fp);
	}
	
	fclose(fp);
	fclose(tmpfp);
	remove("tmp");
	return 0;
    */
}

