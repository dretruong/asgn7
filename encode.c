#include "code.h"
#include "io.h"
#include "trie.h"
#include "word.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
//int compress_stat;
//int uncompressed_stat;
uint16_t bit_length(uint32_t input){
    return log2(input) + 1;
}
void curr_sym(int infile){
    uint8_t curr_sym = 0;
    while(read_sym(infile,&curr_sym)== true){
      printf("Current Symbol = %c\n",curr_sym);
    }
}
void compress(int infile, int outfile){
    TrieNode *root = trie_create();
    TrieNode *curr_node = root;
    TrieNode *prev_node = NULL;
    uint8_t curr_sym = 0;
    uint8_t prev_sym = 0;
    uint16_t next_code = START_CODE;
    while(read_sym(infile,&curr_sym)== true){
        //printf("Current Symbol = %c\n",curr_sym);
        if(curr_node){
          TrieNode *next_node = trie_step(curr_node,curr_sym);
          if(next_node != NULL){
              prev_node = curr_node;
              curr_node = next_node;
          }
          else{
              buffer_pair(outfile,curr_node->code,curr_sym,bit_length(next_code));
              curr_node->children[curr_sym] = trie_node_create(next_code);
              curr_node = root;
              next_code +=1;
          }
          if(next_code == MAX_CODE){
              trie_reset(root);
              curr_node = root;
              next_code = START_CODE;
          }
          prev_sym = curr_sym;
        }
    }
    if(curr_node != root){
        buffer_pair(outfile,prev_node->code,prev_sym,bit_length(next_code));
        next_code = (next_code+ 1) % MAX_CODE;
    }
    trie_delete(root);
    buffer_pair(outfile,STOP_CODE,0,bit_length(next_code));
    flush_pairs(outfile);
}
int main(int argc, char **argv) {
  bool stat_display = false;
  int infile = STDIN_FILENO;
  int outfile = STDOUT_FILENO;
  int option = 0;
  bool I = false;
  bool O = false;
  char *m = NULL;
  char *m1 = NULL;
  while ((option = getopt(argc, argv, "vi:o:")) != -1) {
    switch (option) {
    case 'v':
      stat_display = true;
      break;
    case 'i':
      I = true;
      //strcpy(m,optarg);
      //infile = open(optarg, O_RDONLY);
      m = strdup(optarg);
      break;
    case 'o':
      O = true;
      //strcpy(m1,optarg);
      m1 = strdup(optarg);
      //outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      break;
    }
  }
  if(I == true){
    infile = open(m, O_RDONLY);
  }
  if(O == true){
    outfile = open(m1, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  }
  struct stat infilestats;
  fstat(infile,&infilestats);
  fchmod(outfile,infilestats.st_mode);
  FileHeader h = {0,0};
  h.magic = 0x8badbeef;
  h.protection = infilestats.st_mode;
  write_header(outfile,&h);
  fchmod(outfile,infilestats.st_mode);
  compress(infile,outfile);
  close(infile);
  close(outfile);
  free(m);
  free(m1);
  if(stat_display == true){
    printf("Compressed file size: %f bytes\n", compress_stat);
    printf("Uncompressed file size: %f bytes\n", uncompressed_stat);
    float compres_rat = (100 * (1 - (compress_stat/ uncompressed_stat)));
    printf("Compression ratio: %.2f%%\n", compres_rat);
  }
}
