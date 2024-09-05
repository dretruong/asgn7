#include "trie.h"

TrieNode *trie_node_create(uint16_t index){
    TrieNode *n = calloc(1,sizeof(TrieNode));
    if(n){
      for(int i = 0; i < 256; i++){
          n->children[i] = NULL;
      } 
    n->code = index;
    }
    return n;
    
    
}

void trie_node_delete(TrieNode *n){
    free(n);
    return;
}

TrieNode *trie_create(void){
    return trie_node_create(EMPTY_CODE);
}

void trie_reset(TrieNode *root){
    for(int i = 0; i < 256; i++){
        if(root->children[i]){
            trie_delete(root->children[i]);
        }
    }
}

void trie_delete(TrieNode *n){
    if(!n){
        return;
    }
    for(int i = 0; i <256; i++){
        trie_delete(n->children[i]);
    }
    trie_node_delete(n);
}

TrieNode *trie_step(TrieNode *n, uint8_t sym){
    return n->children[sym];
}

