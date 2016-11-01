#ifndef _TRIE_H_
#define _TRIE_H_

typedef struct trie_s trie_t;
typedef int (*getpos_pf)(char c);
typedef int (*getchar_pf)(int i);

trie_t *trie_init(const char *charset, getpos_pf getpos, getchar_pf mgetchar);

void trie_free(trie_t *trie);

int trie_insert(trie_t *trie, const char *word);

int trie_has_item(trie_t *trie, const char *word);

void trie_print(trie_t *trie);

#endif