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
uint16_t bit_length(uint32_t input){
    
    return log2(input) + 1;
}

void decompress(int infile, int outfile){
    WordTable *table = wt_create();
    if(!table){
      free(table);
      return;
    }
    uint8_t curr_sym = 0;
    uint16_t curr_code = 0;
    uint32_t next_code = START_CODE;
    while(read_pair(infile,&curr_code,&curr_sym,bit_length(next_code))){
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        buffer_word(outfile,table[next_code]);
        next_code = next_code + 1;
        if(next_code == MAX_CODE){
            wt_reset(table);
            next_code = START_CODE;
        }
    }
    wt_delete(table);
    flush_words(outfile);
}

int main(int argc, char **argv) {
  bool stat_display = false;
  int infile = STDIN_FILENO;
  int outfile = STDOUT_FILENO;
  int option = 0;
  while ((option = getopt(argc, argv, "vi:o:")) != -1) {
    switch (option) {
    case 'v':
      stat_display = true;
      break;
    case 'i':
      infile = open(optarg,O_RDONLY);
      break;
    case 'o':
      outfile = open(optarg,O_WRONLY|O_CREAT|O_TRUNC,0644);
      break;
    }
  }
  FileHeader h;
  read_header(infile,&h);
  h.magic = 0x8badbeef;
  //h.protection = stat.st_mode;
  decompress(infile,outfile);
  //printf("iters = %d\n", iter);
  close(infile);
  close(outfile);
  if(stat_display == true){
    printf("Compressed file size: %f bytes\n", uncompressed_stat);
    printf("Uncompressed file size: %f bytes\n", compress_stat);
    float compres_rat = (100 * (1 - (uncompressed_stat/ compress_stat)));
    printf("Compression ratio: %.2f%%\n", compres_rat);
  }
}

