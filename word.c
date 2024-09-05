#include "word.h"

Word *word_create(uint8_t *syms, uint64_t len){
    Word *w = malloc(sizeof(Word));
    if(!w){
      free(w);
    }
    else{
      w->syms = malloc(sizeof(syms));
      if(w->syms){
        w->len = len;
        for(uint64_t i = 0; i < len; i ++){
             w->syms[i] = syms[i];  
        }
      }
    }
    return w;
}

Word *word_append_sym(Word *w, uint8_t sym){
    if(w == NULL){
      Word *x = word_create(&sym,1);
      return x;
    }
    Word *temp = malloc(sizeof(Word)+1);
    if(temp){
      temp->syms = malloc(sizeof(Word)+1);
      temp->len = w->len + 1;
      for(uint32_t i = 0; i < temp->len; i++){
          temp->syms[i] = w->syms[i];
      }
      temp->syms[w->len] = sym;
      Word *apnd = word_create(temp->syms,w->len+1);
      word_delete(temp);
      return apnd;
    }
    else{
      return temp;
    }
}

void word_delete(Word *w){
    free(w->syms);
    free(w);
    return;
}

WordTable *wt_create(void){
    WordTable *t = calloc(MAX_CODE, sizeof(Word));
    //if(t){
      //t[EMPTY_CODE] = word_create(0,0);
    //}
    return t;
}

void wt_reset(WordTable *wt){
    for(int i = START_CODE; i < MAX_CODE; i++){
        word_delete(wt[i]);
    }
}

void wt_delete(WordTable *wt){
    for(uint32_t i = 0; i <MAX_CODE; i++){
        if(wt[i] != NULL){
            word_delete(wt[i]);
        }
    }
    free(wt);
}



