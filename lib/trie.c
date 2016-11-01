#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

#define NO	0
#define YES	1

struct trie_node_s{
	char isword;
	struct trie_node_s **node;
};

struct trie_s{
	char *charset;
	int charsetlen;
	getpos_pf getpos;
	getchar_pf mgetchar;
	struct trie_node_s *trie;
};

static struct trie_node_s * make_node(int count)
{
	struct trie_node_s *node = NULL;
	int i;
	node = (struct trie_node_s *)malloc(sizeof(struct trie_node_s));
	if (node) {
		node->node = (struct trie_node_s **)malloc(sizeof(struct trie_node_s *)*count);
		node->isword = NO;
		for(i = 0; i < count;i++) {
			node->node[i] = NULL;
		}
	}
	return node;
}

static void node_free(struct trie_node_s *trie_node, int count)
{
	int i;

	for (i = 0;i < count; i++) {
		if (trie_node->node[i] != NULL) {
			node_free(trie_node->node[i],count);
			trie_node->node[i] = NULL;
		}
	}
	free(trie_node);
}

trie_t *trie_init(const char *charset, getpos_pf getpos, getchar_pf mgetchar)
{
	trie_t *trie = NULL;

	trie = (trie_t *)malloc(sizeof(struct trie_s));
	if (trie) {
		trie->charset = strdup(charset);
		trie->charsetlen = strlen(charset);
		trie->getpos = getpos;
		trie->mgetchar = mgetchar;
		trie->trie = make_node(trie->charsetlen);
	}
	return trie;
}

void trie_free(trie_t *trie)
{
	free(trie->charset);
	node_free(trie->trie, trie->charsetlen);
	free(trie);
}

static void insert(trie_t *trie, struct trie_node_s *node,const char *word)
{
	char c = *word;
	int i = trie->getpos(c);
	if(strlen(word) == 1) {
		if (!node->node[i]){
			node->node[i] = make_node(trie->charsetlen);
		}
		node->node[i]->isword = YES;
	} else {
		if (!node->node[i]) {
			node->node[i] = make_node(trie->charsetlen);
		}
		insert(trie, node->node[i], word+1);
	}
}

int trie_insert(trie_t *trie, const char *word)
{
	insert(trie, trie->trie,word);
	return 0;
}

static int is_in_trie(trie_t *trie, struct trie_node_s *node,const char *word)
{
	char c = *word;
	int i = trie->getpos(c);

	if (strlen(word) == 1) {
		if (node->node[i]->isword == YES) {
			return 1;
		} else {
			return 0;
		}
	} else {
		if (node->node[i]) {
			return is_in_trie(trie,node->node[i],word+1);
		} else {
			return 0;
		}
	}
}

int trie_has_item(trie_t *trie, const char *word)
{
	return is_in_trie(trie, trie->trie,word);
}

static void print_item(trie_t *trie,struct trie_node_s *node,char *p)
{
	int i;
	for (i = 0;i < trie->charsetlen;i++) {
		if(node->node[i] != NULL) {
			printf("%c", trie->mgetchar(i));
			if (node->node[i]->isword == YES) {
				printf("\n");
			}
			print_item(trie,node->node[i], p);
		}
	}
}

void trie_print(trie_t *trie)
{
	char buf[1024];
	memset(buf,0,1024);
	print_item(trie,trie->trie, buf);
}